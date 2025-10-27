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
    int start_time;
    int remaining_time;
    int last_run_time;
    int remaining_quantum;
    string state; // e.g., "READY", "RUNNING", "WAITING", "TERMINATED"
    // feel free to add more variables here
};

void printPCBState(const PCB& process, int start_time, int current_time) {
    if (process.id == "Idle") {
        cout << "Time " << start_time << "-" << current_time << ": Idle";
    } else {
        cout << "Time " << start_time << "-" << current_time << ": " << process.id
            << " (Priority " << process.priority << ")";
    }
    cout << endl;
}

void sortByPriority(vector<PCB>& processes, bool stable = false) {
    // This function will sort the processes based on priority
    // Higher priority value means higher priority
    // lambda function to compare two PCBs based on priority
    if (stable) {
        stable_sort(processes.begin(), processes.end(), [](const PCB& a, const PCB& b) {
            if (a.priority == b.priority) {
                return a.arrival_time < b.arrival_time; // If priorities are equal, sort by arrival time
            }
            return a.priority > b.priority;
        });
    } else {
        sort(processes.begin(), processes.end(), [](const PCB& a, const PCB& b) {
        if (a.priority == b.priority) {
            return a.arrival_time < b.arrival_time; // If priorities are equal, sort by arrival time
        }
        return a.priority > b.priority;
        });
    }

}

void scheduleProcesses(vector<PCB>& processes, int time_quantum) {
    int current_time = 0;
    // TODO: initialize ready queue
    // run process in ready queue up to time quantum or until completion
    // implement preemptive scheduling based on priority and time quantum
    // implement round-robin for processes with same priority
    vector<PCB> ready_queue;
    PCB idle_process;
    idle_process.id = "Idle";
    idle_process.state = "WAITING";
    string last_process_id = "";

    PCB* current_process = nullptr;
    int process_start_time = 0;
   // Continue scheduling until all processes are terminated
    while (true) {
        // Check if all processes are terminated
        bool all_done = true;
        for (const auto& p: processes) {
            if (p.state != "TERMINATED") {
                all_done = false;
                break;
            }
        }
        if (all_done) {
            // Print the final running processes if any
            if (current_process != nullptr) {
                printPCBState(*current_process, process_start_time, current_time);
            } else if (idle_process.state == "RUNNING") {
                printPCBState(idle_process, idle_process.start_time, current_time);
            }
            break;
        }
        // Add newly arrived processes to ready queue
        for (int i = 0; i < processes.size(); ++i) {
            if (processes[i].arrival_time == current_time && processes[i].state != "TERMINATED") {
                processes[i].state = "READY";
                bool in_ready_queue = false;
                for (const auto& p: ready_queue) {
                    if (p.id == processes[i].id) {
                        in_ready_queue = true;
                    }
                }
                if (!in_ready_queue) {
                    ready_queue.push_back(processes[i]);
                }
            }
        }
        // Check for preemption
        if (!ready_queue.empty()) {
            // handle idle process state change
            sortByPriority(ready_queue);
            // check if current process already in ready queue
            

            if (current_process != nullptr) {
                if (ready_queue[0].priority > current_process->priority) {
                    // Preempt current process
                    printPCBState(*current_process, process_start_time, current_time);

                    // Add current process back to ready queue
                    current_process->state = "READY";
                    ready_queue.push_back(*current_process);
                    sortByPriority(ready_queue);

                    // Update the process in main vector
                    for (auto& p: processes) {
                        if (p.id == current_process->id) {
                            p = *current_process;
                            break;
                        }
                    }

                    current_process = nullptr;
                }
            }
        } 

        // Select the process to run
        if (current_process == nullptr && !ready_queue.empty()) {
            // stop idle process if running
            if (idle_process.state == "RUNNING") {
                printPCBState(idle_process, idle_process.start_time, current_time);
                idle_process.state = "WAITING";
            }

            // Get the highest priority process
            current_process = new PCB(ready_queue[0]);
            ready_queue.erase(ready_queue.begin());
            
            // only update start time if it's a different process

            current_process->state = "RUNNING";
            if (last_process_id != current_process->id) {
                process_start_time = current_time;
            }
            last_process_id = current_process->id;
            
        }

        // run current process for 1 time unit
        if (current_process != nullptr) {
            current_process->remaining_time--;
            current_process->remaining_quantum--;
            
            // check if process is completed
            if (current_process->remaining_time == 0) {
                current_process->last_run_time = current_time;

                current_process->state = "TERMINATED";
                printPCBState(*current_process, process_start_time, current_time + 1);
                // Update the process in main vector
                for (auto& p: processes) {
                    if (p.id == current_process->id) {
                        p = *current_process;
                        break;
                    }
                }

                delete current_process;
                current_process = nullptr;
            } else if (current_process->remaining_quantum == 0) {
                current_process->remaining_quantum = time_quantum;

                // put it back into ready queue only if it still needs CPU time
                if (current_process->remaining_time > 0) {
                    current_process->state = "READY";
                }
                ready_queue.push_back(*current_process);
                sortByPriority(ready_queue, true);
                // only print if context switch occurs
                bool context_switch = true;
                if (!ready_queue.empty() && ready_queue[0].id == current_process->id) {
                    context_switch = false;
                }
                if (context_switch) {
                    printPCBState(*current_process, process_start_time, current_time + 1);
                }

                // Update the process in main vector
                for (auto& p: processes) {
                    if (p.id == current_process->id) {
                        p = *current_process;
                        break;
                    }
                }
                delete current_process;
                current_process = nullptr;
            }
        } else {
            // CPU is idle
            if (idle_process.state != "RUNNING") {
                idle_process.state = "RUNNING";
                idle_process.start_time = current_time;
            }
        }
        current_time++;
    } 
}

void printTurnOverStats(const vector<PCB>& processes) {
    cout << "\nTurnaround Time\n";
    for (const auto& p : processes) {
        int turnaround_time = p.last_run_time - p.arrival_time + 1;
        cout << p.id << " = " << turnaround_time << endl;
    }
}

void printWaitingTimeStats(const vector<PCB>& processes) {
    cout << "\nWaiting Time\n";
    for (const auto& p : processes) {
        int turnaround_time = p.last_run_time - p.arrival_time + 1;
        int waiting_time = turnaround_time - p.burst_time;
        cout << p.id << " = " << waiting_time << endl;
    }
}

void printCPUUtilizationStats(const vector<PCB>& processes, int time_quantum) {
    int total_burst_time = 0;
    for (const auto& p : processes) {
        total_burst_time += p.burst_time;
    }
    // Assuming total time is the last process's completion time
    int total_time = 0;
    for (const auto& p : processes) {
        if (p.last_run_time + 1 > total_time) {
            total_time = p.last_run_time + 1;
        }
    }
    double cpu_utilization = (static_cast<double>(total_burst_time) / total_time) * 100.0;
    cout << "\nCPU Utilization Time\n" << total_burst_time << "/" << total_time << endl;
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
    printTurnOverStats(processes);
    printWaitingTimeStats(processes);
    printCPUUtilizationStats(processes, time_quantum);
    return 0;
}
