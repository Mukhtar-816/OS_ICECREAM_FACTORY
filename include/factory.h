#ifndef FACTORY_H
#define FACTORY_H

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

#define MAX_WORKERS 15
#define MAX_LOG_ENTRIES 10

typedef enum {
    READY,
    RUNNING,
    WAITING,
    DISPATCHING,
    CRITICAL_SECTION
} WorkerStatus;

typedef struct {
    int id;
    char type[20];
    WorkerStatus status;
    float progress;
    double wait_start_time;
    double current_wait_duration;
    char holding_resource[30];
} WorkerState;

typedef struct {
    WorkerState workers[MAX_WORKERS];
    int num_workers;
    
    // Resources
    int available_vats;
    int available_freezers;
    int available_packaging_stations;
    
    int total_produced;
    double start_time;
    float batches_per_minute;
    
    char mutex_owner[30];
    bool mutex_locked;
    
    char logs[MAX_LOG_ENTRIES][100];
    int log_count;
} FactoryState;

// Global Synchronization
extern pthread_mutex_t gui_mutex;
extern sem_t sem_vats;
extern sem_t sem_freezers;
extern sem_t sem_packaging;
extern sem_t sem_ready_to_freeze;
extern sem_t sem_ready_to_pack;

// Shared State
extern volatile FactoryState global_state;

void add_log(const char* message);
void update_worker(int id, WorkerStatus status, float progress);

#endif
