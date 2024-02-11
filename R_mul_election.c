#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define MAX 10
int list[MAX], n, c;
pthread_mutex_t state_mutex;
pthread_cond_t state_cond;

enum ProcessState { IDLE, CANDIDATE, LEADER };

struct Process {
    int id;
    pthread_cond_t state_cond;
    enum ProcessState state;
    int highest_seen_id;
};

struct Process processes[MAX];
int activate_id,crash_id;
void display() {
    int i;
    printf("\nProcesses-->");
    for (i = 1; i <= n; i++) {
        printf("\t %d", i);
    }
    printf("\nAlive-->");
    for (i = 1; i <= n; i++) {
        printf("\t %d", list[i]);
    }
    printf("\nCoordinator is::%d", c);
}

void receive_election_message(int receiver_id, int sender_id) {
    if (processes[receiver_id].state == IDLE || processes[receiver_id].state == CANDIDATE) {
        printf("Process %d receives election message from %d\n", receiver_id, sender_id);

        pthread_mutex_lock(&state_mutex);
        processes[receiver_id].highest_seen_id = sender_id;

        // Multiple initiation handling: Only the process with the highest ID becomes CANDIDATE
        if (processes[receiver_id].id > sender_id && processes[receiver_id].state == IDLE) {
            processes[receiver_id].state = CANDIDATE;
            pthread_cond_broadcast(&state_cond);
        }
        pthread_mutex_unlock(&state_mutex);
    }
}

void send_election_message(int sender_id, int receiver_id) {
    // Replace with your actual network communication implementation
    printf("Process %d sends election message to %d (simulated)\n", sender_id, receiver_id);
}

void send_alive_message(int sender_id, int receiver_id) {
    // Replace with your actual network communication implementation
    printf("Process %d sends alive message to %d (simulated)\n", sender_id, receiver_id);
}

void receive_alive_message(int receiver_id, int sender_id) {
    if (processes[receiver_id].state == CANDIDATE) {
        printf("Process %d receives alive message from %d\n", receiver_id, sender_id);

        pthread_mutex_lock(&state_mutex);
        if (sender_id > processes[receiver_id].highest_seen_id) {
            processes[receiver_id].state = IDLE;
            pthread_cond_broadcast(&processes[receiver_id].state_cond);
        }
        pthread_mutex_unlock(&state_mutex);
    }
}

void election_phase() {
    // Use state_mutex and state_cond for thread-safe election coordination
    pthread_mutex_lock(&state_mutex);

    int active_candidate = -1;
    for (int i = 0; i < n; i++) {
        if (processes[i].state == CANDIDATE) {
            if (active_candidate == -1 || processes[i].id > processes[active_candidate].id) {
                active_candidate = i;
            } else {
                processes[i].state = IDLE;
                pthread_cond_broadcast(&processes[i].state_cond);
            }
        }
    }

    if (active_candidate != -1) {
        processes[active_candidate].state = LEADER;
        c = processes[active_candidate].id;
        pthread_cond_broadcast(&processes[active_candidate].state_cond);
        send_alive_message(c, (c + 1) % n);
    }

    pthread_mutex_unlock(&state_mutex);
}

void leader_phase() {
    // Implement leader-specific tasks here
    printf("Leader: Process %d\n", c);
    sleep(rand() % 5); // Simulate leader work for a random duration
}

void* process_behavior(void* arg) {
    int process_id = *((int*)arg);

    while (1) {
        switch (processes[process_id].state) {
            case IDLE:
                // Initiate election with a certain probability or based on specific events
                if (rand() % 10 == 0) { // Simulate 10% chance of starting an election
                    pthread_mutex_lock(&state_mutex);
                    processes[process_id].state = CANDIDATE;
                    processes[process_id].highest_seen_id = process_id;
                    pthread_cond_broadcast(&state_cond);
                    pthread_mutex_unlock(&state_mutex);
                    send_election_message(process_id, (process_id + 1) % n);
                    printf("Process %d starts an election\n", process_id);
                }
                break;
            case CANDIDATE:
                // Receive and handle messages and transitions:
                // - If receive election message with higher ID, become IDLE
                // - If receive alive message from higher ID, become IDLE
                // - Wait on condition variable for further processing or transition
                pthread_mutex_lock(&state_mutex);
                while (processes[process_id].state == CANDIDATE) {
                    pthread_cond_wait(&state_cond, &state_mutex);
                }
                pthread_mutex_unlock(&state_mutex);
                break;
            case LEADER:
                // Perform leader tasks (implement your specific logic here)
                printf("Process %d is the leader\n", process_id);
                for (int i = 0; i < 5; i++) { // Simulate leader work
                    sleep(1); // Sleep for 1 second
                    printf("Leader %d doing work...\n", process_id);
                }
                // Simulate leader sending alive messages periodically
                pthread_mutex_lock(&state_mutex);
                send_alive_message(process_id, (process_id + 1) % n);
                pthread_mutex_unlock(&state_mutex);
                sleep(2); // Simulate sending ALIVE every 2 seconds
                break;
        }
    }

    return NULL;
}
int main() {
    srand(time(NULL)); // Seed random number generator

    // Initialize processes and mutexes/conditions
    for (int i = 0; i < MAX; i++) {
        processes[i].id = i;
        processes[i].state = IDLE;
        processes[i].highest_seen_id = -1;
    }
    pthread_mutex_init(&state_mutex, NULL);
    pthread_cond_init(&state_cond, NULL);

    // Get number of processes, validate input
    do {
        printf("Enter the number of processes (1-%d): ", MAX);
        scanf("%d", &n);
    } while (n < 1 || n > MAX);

    // Initialize process states and alive list
    for (int i = 0; i < n; i++) {
        printf("Enter whether process %d is alive (1/0): ", i + 1);
        scanf("%d", &list[i]);
        if (list[i]) {
            processes[i].state = IDLE;
            c = i; // Initial coordinator is the first alive process
        }
    }

    // Display initial state
    display();

    // Create threads for each process
    pthread_t threads[n];
    int process_ids[n];
    for (int i = 0; i < n; i++) {
        process_ids[i] = i;
        pthread_create(&threads[i], NULL, process_behavior, &process_ids[i]);
    }

    // Main loop for interaction and monitoring
    int choice;
    do {
        printf("\nMenu:\n");
        printf("1. Display processes\n");
        printf("2. Crash a process\n");
        printf("3. Activate a process\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                display();
                break;
            case 2:

                do {
                    printf("Enter process ID to crash (1-%d): ", n);
                    scanf("%d", &crash_id);
                } while (crash_id < 1 || crash_id > n || !list[crash_id - 1]);
                if (processes[crash_id - 1].state == LEADER) {
                    printf("Cannot crash the leader, please activate another process first.\n");
                } else {
                    list[crash_id - 1] = 0; // Mark process as dead
                    printf("Process %d crashed\n", crash_id);
                }
                break;
            case 3:


                do {
                    printf("Enter process ID to activate (1-%d): ", n);
                    scanf("%d", &activate_id);
                } while (activate_id < 1 || activate_id > n || list[activate_id - 1]);
                list[activate_id - 1] = 1; // Mark process as alive
                printf("Process %d activated\n", activate_id);
                // Signal potential state changes for other processes
                pthread_mutex_lock(&state_mutex);
                processes[activate_id - 1].state = IDLE;
                pthread_cond_broadcast(&state_cond);
                pthread_mutex_unlock(&state_mutex);
                break;
            case 4:
                // Exit
                for (int i = 0; i < n; i++) {
                    pthread_join(threads[i], NULL);
                }
                pthread_mutex_destroy(&state_mutex);
                pthread_cond_destroy(&state_cond);
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 4);

    return 0;
}
