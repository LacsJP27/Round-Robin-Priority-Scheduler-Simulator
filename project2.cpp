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
    string state; // e.g., "READY", "RUNNING", "WAITING", "TERMINATED"
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
   
    // Track consecutive process execution
    int consecutive_start_time = 0;
    string last_process_id = "";

    while (true) {
        // check if all process are completed
        bool all_done = true;
        for (int i = 0; i < processes.size(); i++) {
            if (processes[i].state != "TERMINATED") {
                all_done = false;
                break;
            }
        }
        if (all_done) {
            break;
        }

        // Add newly arrived processes to ready queue
        for (int i = 0; i < processes.size(); i++) {
            if (processes[i].state != "TERMINATED" && processes[i].arrival_time == current_time) {
                // Check if process is not already in ready queue
                bool already_in_queue = false;
                for (const auto& p : ready_queue) {
                    if (p.id == processes[i].id) {
                        already_in_queue = true;
                        break;
                    }
                }
                if (!already_in_queue) {
                    processes[i].state = "READY";
                    ready_queue.push_back(processes[i]);
                    sortByPriority(ready_queue);
                }
            }
        }

        if (!ready_queue.empty()) {
            // Get the highest priority process
            PCB current_process = ready_queue.front();
            ready_queue.erase(ready_queue.begin());
            current_process.state = "RUNNING";
            
            // Check if this is a different process than the last one
            if (current_process.id != last_process_id) {
                // Print the previous process if it exists
                if (last_process_id != "") {
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

            // Execute process for minimum of (remaining_time, time_quantum)
            current_time ++;
            current_process.remaining_time--;
            current_process.remaining_quantum--;

            // TODO: if process complete then interrupt
            if (current_process.remaining_time == 0) {
                current_process.state = "TERMINATED";
                // Process will be printed when we detect the switch (next iteration or end)
            } else if (current_process.remaining_quantum == 0) {
                // Quantum expired - process switches out
                current_process.state = "READY";
                current_process.remaining_quantum = time_quantum;  // Reset quantum
                ready_queue.push_back(current_process);
                sortByPriority(ready_queue);  // Sort after adding
            } 
            // update remaining_time in the original vector
            for (auto& p : processes) {
                if (p.id == current_process.id) {
                    p.remaining_time = current_process.remaining_time;
                    p.state = current_process.state;
                    break;
                }
            }
        } else {
            if (last_process_id != "") {
                if (last_process_id != "IDLE") {
                    // Find the process details for printing
                    for (const auto& p : processes) {
                        if (p.id == last_process_id) {
                            PCB temp_process = p;
                            printPCBState(temp_process, consecutive_start_time, current_time);
                            break;
                        }
                    }
                    last_process_id = "IDLE";
                    consecutive_start_time = current_time;
                }
            }
            current_time++;
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
        p.state = "WAITING"; 
        processes.push_back(p);
        // Set initial state
        
    }

    // TODO: Create CPU scheduler simulator
    scheduleProcesses(processes, time_quantum);
    // You can create any data structures, classes, functions helpers as you wish
    // Do not forget to include comments describing how your simulator works.
    return 0;
}
