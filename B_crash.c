#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_PROCESSES 5

enum ProcessState { IDLE, ELECTED, LOST };

struct Process {
    int id;
    enum ProcessState state;
};

struct Process processes[NUM_PROCESSES];

void initialize_processes() {
    for (int i = 0; i < NUM_PROCESSES; i++) {
        processes[i].id = i;
        processes[i].state = IDLE;
    }
}

void election(int pid) {
    printf("Process %d starts election\n", pid);
    processes[pid].state = ELECTED;

    for (int i = pid + 1; i < NUM_PROCESSES; i++) {
        if (processes[i].state == IDLE) {
            printf("Process %d sends election message to process %d\n", pid, i);
            // Simulate network communication
            // In real implementation, send message to process i
        }
    }

    sleep(2); // Simulate message propagation time

    int new_leader = pid;
    for (int i = pid + 1; i < NUM_PROCESSES; i++) {
        if (processes[i].state == ELECTED) {
            printf("Process %d receives election message from process %d\n", pid, i);
            if (i > new_leader) {
                new_leader = i;
            }
        }
    }

    if (new_leader == pid) {
        printf("Process %d becomes the coordinator\n", pid);
        // Coordinator-specific tasks here
    } else {
        printf("Process %d acknowledges process %d as the new coordinator\n", pid, new_leader);
        // Send acknowledgment message to new_leader
        processes[pid].state = LOST;
    }

    // Check if process 4 is alive and has a higher priority
    if (pid != NUM_PROCESSES - 1 && processes[NUM_PROCESSES - 1].state == ELECTED) {
        printf("Process %d receives alive message from process %d\n", pid, NUM_PROCESSES - 1);
        printf("Process %d acknowledges process %d as the new coordinator\n", pid, NUM_PROCESSES - 1);
        processes[pid].state = LOST;
    }
}

int main() {
    initialize_processes();

    printf("Enter the IDs of crashed processes (0 to 4), separated by space: ");
    int crash_pid1, crash_pid2;
    scanf("%d %d", &crash_pid1, &crash_pid2);

    processes[crash_pid1].state = LOST;
    processes[crash_pid2].state = LOST;

    printf("Enter the ID of the initiating process (0 to 4): ");
    int initiating_pid;
    scanf("%d", &initiating_pid);

    while (processes[initiating_pid].state == LOST) {
        printf("Process %d is crashed. Please enter a valid ID: ", initiating_pid);
        scanf("%d", &initiating_pid);
    }

    election(initiating_pid);

    return 0;
}
