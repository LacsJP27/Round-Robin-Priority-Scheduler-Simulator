// Structure to represent a process control block
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <queue>
using namespace std;
struct PCB {
    string id;
    int priority;
    int burst_time;
    int arrival_time;
    int remaining_time;
    int last_run_time;
    int remaining_quantum;
    // feel free to add more variables here
};

void printPCBState(const PCB& process, int start_time, int current_time) {
    if (process.id == "IDLE") {
        cout << "Time " << start_time << "-" << current_time << ": IDLE";
    } else {
        cout << "Time " << start_time << "-" << current_time << ": " << process.id
            << "(Priority " << process.priority << ")";
    }
    cout << endl;
}

void sortByPriority(vector<PCB>& processes) {
    // This function will sort the processes based on priority
    // Higher priority value means higher priority
    // lambda function to compare two PCBs based on priority
    sort(processes.begin(), processes.end(), [](const PCB& a, const PCB& b) {
        if (a.priority == b.priority) {
            return a.remaining_time > b.remaining_time; // If priorities are equal, sort by arrival time
        }
        return a.priority > b.priority;
    });
}

void scheduleProcesses(vector<PCB>& processes, int time_quantum) {
    // This function will implement the CPU scheduling simulation.
    // You can choose any scheduling algorithm (e.g., Round Robin, Priority Scheduling, etc.)
    // For demonstration, let's implement a simple Round Robin scheduling.
    int current_time = 0;
    // Processes that are ready to run
    vector<PCB> ready_queue;
    // Track the completion status of processes
    vector<bool> completed(processes.size(), false);
    
    // Track consecutive process execution
    string last_process_id = "";
    int consecutive_start_time = 0;

    while (true) {
        // check if all process are completed
        bool all_done = true;
        for (int i = 0; i < completed.size(); i++) {
            if (!completed[i]) {
                all_done = false;
                break;
            }
        }
        if (all_done) {
            // Print the last process if it exists
            if (!last_process_id.empty()) {
                for (const auto& p : processes) {
                    if (p.id == last_process_id) {
                        PCB temp_process = p;
                        printPCBState(temp_process, consecutive_start_time, current_time);
                        break;
                    }
                }
            }
            break;
        }

        // Add newly arrived processes to ready queue (only those arriving exactly at current_time)
        for (int i = 0; i < processes.size(); i++) {
            if (!completed[i] && processes[i].arrival_time == current_time) {
                // Check if process is not already in ready queue
                bool already_in_queue = false;
                for (const auto& p : ready_queue) {
                    if (p.id == processes[i].id) {
                        already_in_queue = true;
                        break;
                    }
                }
                if (!already_in_queue) {
                    ready_queue.push_back(processes[i]);
                }
            }
        }

        // sort the ready queue by priority
        sortByPriority(ready_queue);

        if (!ready_queue.empty()) {
            // Get the highest priority process
            PCB& current_process = ready_queue.front();
            
            // Check if this is a different process than the last one
            if (current_process.id != last_process_id) {
                // Print the previous process if it exists
                if (!last_process_id.empty()) {
                    // Find the process details for printing
                    for (const auto& p : processes) {
                        if (p.id == last_process_id) {
                            PCB temp_process = p;
                            printPCBState(temp_process, consecutive_start_time, current_time);
                            break;
                        }
                    }
                }
                // Start tracking new process
                last_process_id = current_process.id;
                consecutive_start_time = current_time;
            }
            
            int start_time = current_time;

            // Execute process one time unit at a time to check for preemption
            int exec_time = min(current_process.remaining_time, time_quantum);
            int time_executed = 0;
            bool preempted = false;
            
            while (time_executed < exec_time && !preempted) {
                // Execute for 1 time unit
                current_process.remaining_time--;
                current_time++;
                time_executed++;
                
                // Check for newly arrived processes that might preempt
                for (int i = 0; i < processes.size(); i++) {
                    if (!completed[i] && processes[i].arrival_time == current_time) {
                        // Check if this new process has higher priority
                        if (processes[i].priority > current_process.priority) {
                            // Check if it's not already in ready queue
                            bool already_in_queue = false;
                            for (const auto& p : ready_queue) {
                                if (p.id == processes[i].id) {
                                    already_in_queue = true;
                                    break;
                                }
                            }
                            if (!already_in_queue) {
                                ready_queue.push_back(processes[i]);
                                preempted = true;
                                break;
                            }
                        }
                    }
                }
            }


            // update remaining_time in the original vector
            for (auto& p : processes) {
                if (p.id == current_process.id) {
                    p.remaining_time = current_process.remaining_time;
                    break;
                }
            }
            
            // mark process as completed if done
            if (current_process.remaining_time == 0) {
                for (int i = 0; i < completed.size(); i++) {
                    if (processes[i].id == current_process.id) {
                        completed[i] = true;
                        break;
                    }
                }
                // Print the completed process
                printPCBState(current_process, consecutive_start_time, current_time);
                last_process_id = ""; // Reset since process is done
                
                // remove from ready queue
                ready_queue.erase(ready_queue.begin());
            } else if (preempted) {
                // Process was preempted, print current execution and put back in queue
                printPCBState(current_process, consecutive_start_time, current_time);
                last_process_id = ""; // Reset for next process
                
                // Update the process in ready queue and move to appropriate position
                ready_queue.erase(ready_queue.begin());
                ready_queue.push_back(current_process);
            } else {
                // Process used full quantum, move to end of queue (round robin)
                ready_queue.push_back(current_process);
                ready_queue.erase(ready_queue.begin());
            }
        } else {
            // Print the previous process if it exists before going idle
            if (!last_process_id.empty()) {
                for (const auto& p : processes) {
                    if (p.id == last_process_id) {
                        PCB temp_process = p;
                        printPCBState(temp_process, consecutive_start_time, current_time);
                        break;
                    }
                }
                last_process_id = "";
            }
            
            // Find next arrival time
            int next_arrival = -1;
            for (int i = 0; i < processes.size(); i++) {
                if (!completed[i]) {
                    // get the next closest arrival time
                    if (next_arrival == -1 || processes[i].arrival_time < next_arrival) {
                        next_arrival = processes[i].arrival_time;
                    }
                }
            }

            PCB idle_process;
            idle_process.id = "Idle";
            int start_time = current_time;
            current_time = next_arrival;

            printPCBState(idle_process, start_time, current_time);
        }
    }
}
       
int main() {
    string line;
    char dummy;
    int time_quantum;
    vector<PCB> processes;

    // Read time quantum from standard input
    cin >> dummy >> time_quantum;

    // Read processes from standard input
    while (cin >> line) {
        PCB p;
        p.id = line;
        cin >> p.priority >> p.burst_time >> p.arrival_time;
        p.remaining_time = p.burst_time;
        p.last_run_time = -1;
        p.remaining_quantum = time_quantum;
        processes.push_back(p);
    }

    // TODO: Create CPU scheduler simulator
    scheduleProcesses(processes, time_quantum);
    // You can create any data structures, classes, functions helpers as you wish
    // Do not forget to include comments describing how your simulator works.
    return 0;
}
