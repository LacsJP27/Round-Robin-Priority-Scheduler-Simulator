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

    while (true) {
        // check if all process are completed
        bool all_done = true;
        for (int i = 0; i < completed.size(); i++) {
            if (!completed[i]) {
                all_done = false;
                break;
            }
        }
        if (all_done) break;

        // Add newly arrived processes to ready queue
        for (int i = 0; i < processes.size(); i++) {
            if (!completed[i] && processes[i].arrival_time <= current_time) {
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

        if (current_time == 35) {
            // Print the state of the ready queue
            for (const auto& p : ready_queue) {
                cout << "Ready Queue Process: " << p.id << " Priority: " << p.priority << " Remaining Time: " << p.remaining_time << endl;
            }
        }

        // sort the ready queue by priority
        sortByPriority(ready_queue);
        if (current_time == 35) {
            // Print the state of the ready queue
            for (const auto& p : ready_queue) {
                cout << "Ready Queue Process: " << p.id << " Priority: " << p.priority << " Remaining Time: " << p.remaining_time << endl;
            }
        }

        if (!ready_queue.empty()) {
            // current time is gonna be 10
            // Get the highest priority process
            PCB& current_process = ready_queue.front();
            int start_time = current_time;

            // Execute process for minimum of (remaining_time, time_quantum)
            int exec_time = min(current_process.remaining_time, time_quantum);
            current_process.remaining_time -= exec_time;
            current_time += exec_time;

            // TODO: print the process execution state
            printPCBState(current_process, start_time, current_time);
            
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
                // remove from ready queue
                ready_queue.erase(ready_queue.begin());
            } else {
                // cout << "ready_queue.size():" << ready_queue.size() << endl;
                ready_queue.push_back(current_process);
                ready_queue.erase(ready_queue.begin());
            }
        } else {
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
            idle_process.id = "IDLE";
            int start_time = current_time;
            current_time = next_arrival;

            // printPCBState(idle_process, start_time, current_time);
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
