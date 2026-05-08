#include "raylib.h"
#include "../include/factory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

pthread_mutex_t gui_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_vats;
sem_t sem_freezers;
sem_t sem_packaging;
sem_t sem_ready_to_freeze;
sem_t sem_ready_to_pack;
volatile FactoryState global_state;

// --- Helper: Atomic UI Updates ---
void update_ui_status(int id, WorkerStatus status, float progress, const char* resource) {
    pthread_mutex_lock(&gui_mutex);
    if (id >= 0) {
        global_state.workers[id].status = status;
        global_state.workers[id].progress = progress;
        if (resource) strncpy((char*)global_state.workers[id].holding_resource, resource, 29);
        else strcpy((char*)global_state.workers[id].holding_resource, "None");
    }
    pthread_mutex_unlock(&gui_mutex);
}

void add_log(const char* log_msg) {
    pthread_mutex_lock(&gui_mutex);
    if (global_state.log_count < MAX_LOG_ENTRIES) {
        strncpy((char*)global_state.logs[global_state.log_count], log_msg, 99);
        global_state.log_count++;
    } else {
        for (int i = 0; i < MAX_LOG_ENTRIES - 1; i++) {
            strcpy((char*)global_state.logs[i], (char*)global_state.logs[i+1]);
        }
        strncpy((char*)global_state.logs[MAX_LOG_ENTRIES-1], log_msg, 99);
    }
    pthread_mutex_unlock(&gui_mutex);
}

// --- NARRATIVE WORKFLOW LOGIC ---

void* mixer_thread(void* arg) {
    int id = *(int*)arg;
    free(arg);
    char buf[100];
    
    while (1) {
        update_ui_status(id, READY, 0, "Dormant");
        usleep(1000000); 

        // 1. INTENT
        sprintf(buf, "Mixer [%02d]: Requesting Vat Resource...", id);
        add_log(buf);
        update_ui_status(id, WAITING, 0, "Awaiting Vat");

        sem_wait(&sem_vats); 

        // 2. DISPATCHING DELAY (Technical Observability)
        update_ui_status(id, DISPATCHING, 0, "Claiming Vat");
        usleep(1000000); // 1s Scheduling Delay

        // 3. ACQUISITION
        pthread_mutex_lock(&gui_mutex);
        global_state.available_vats--;
        sprintf(buf, "Mixer [%02d]: Captured Vat Resource. Beginning Mix...", id);
        pthread_mutex_unlock(&gui_mutex);
        add_log(buf);

        // 4. EXECUTION (5 Seconds)
        int steps = 25;
        for (int i = 0; i <= steps; i++) {
            update_ui_status(id, RUNNING, (float)i * (100.0f/steps), "Active Mix");
            usleep(200000); // 5s total
        }

        // 5. SIGNALING
        sprintf(buf, "Mixer [%02d]: Batch Ready. Signaling Freezer Pipeline...", id);
        add_log(buf);
        
        pthread_mutex_lock(&gui_mutex);
        global_state.available_vats++;
        pthread_mutex_unlock(&gui_mutex);
        
        sem_post(&sem_vats);
        sem_post(&sem_ready_to_freeze);
        
        update_ui_status(id, READY, 100, "Handover Done");
        usleep(1500000);
    }
    return NULL;
}

void* freezer_thread(void* arg) {
    int id = *(int*)arg;
    free(arg);
    char buf[100];
    
    while (1) {
        update_ui_status(id, READY, 0, "Idle Bridge");

        // 1. SIGNAL WAIT
        update_ui_status(id, WAITING, 0, "Wait Pipeline");
        sem_wait(&sem_ready_to_freeze); 

        sprintf(buf, "Freezer [%02d]: Signal Received. Entering Cooling Stage...", id);
        add_log(buf);

        // 2. RESOURCE WAIT
        update_ui_status(id, WAITING, 0, "Requesting Slot");
        sem_wait(&sem_freezers);

        // 3. DISPATCHING DELAY
        update_ui_status(id, DISPATCHING, 0, "Locking Slot");
        usleep(1000000); 

        // 4. ACQUISITION
        pthread_mutex_lock(&gui_mutex);
        global_state.available_freezers--;
        sprintf(buf, "Freezer [%02d]: Slot Secured. Initiating Thermal Cycle...", id);
        pthread_mutex_unlock(&gui_mutex);
        add_log(buf);

        // 5. EXECUTION (8 Seconds)
        int steps = 40;
        for (int i = 0; i <= steps; i++) {
            update_ui_status(id, RUNNING, (float)i * (100.0f/steps), "Cooling Cycle");
            usleep(200000); // 8s total
        }

        // 6. SIGNALING
        sprintf(buf, "Freezer [%02d]: Cooling Finished. Signaling Packager...", id);
        add_log(buf);
        
        pthread_mutex_lock(&gui_mutex);
        global_state.available_freezers++;
        pthread_mutex_unlock(&gui_mutex);
        
        sem_post(&sem_freezers);
        sem_post(&sem_ready_to_pack);
        
        update_ui_status(id, READY, 100, "Cooling Done");
    }
    return NULL;
}

void* packager_thread(void* arg) {
    int id = *(int*)arg;
    free(arg);
    char buf[100];
    
    while (1) {
        update_ui_status(id, READY, 0, "Idle Station");

        // 1. SIGNAL WAIT
        update_ui_status(id, WAITING, 0, "Wait Pipeline");
        sem_wait(&sem_ready_to_pack);

        sprintf(buf, "Packager [%02d]: Signal Received. Ready to Seal...", id);
        add_log(buf);

        // 2. RESOURCE WAIT
        update_ui_status(id, WAITING, 0, "Request Station");
        sem_wait(&sem_packaging);

        // 3. DISPATCHING DELAY
        update_ui_status(id, DISPATCHING, 0, "Acquiring Station");
        usleep(1000000);

        // 4. ACQUISITION
        pthread_mutex_lock(&gui_mutex);
        global_state.available_packaging_stations--;
        sprintf(buf, "Packager [%02d]: Station Acquired. Finalizing Batch...", id);
        pthread_mutex_unlock(&gui_mutex);
        add_log(buf);

        // 5. EXECUTION (3 Seconds)
        int steps = 15;
        for (int i = 0; i <= steps; i++) {
            update_ui_status(id, RUNNING, (float)i * (100.0f/steps), "Packaging Station");
            usleep(200000); // 3s total
        }

        // 6. COMPLETION
        pthread_mutex_lock(&gui_mutex);
        global_state.total_produced++;
        global_state.available_packaging_stations++;
        sprintf(buf, "Packager [%02d]: Production Finalized. Total: %d", id, global_state.total_produced);
        
        double elapsed = GetTime() - global_state.start_time;
        if (elapsed > 0) {
            global_state.batches_per_minute = (global_state.total_produced / elapsed) * 60.0;
        }
        pthread_mutex_unlock(&gui_mutex);
        add_log(buf);
        
        sem_post(&sem_packaging);
        
        update_ui_status(id, READY, 100, "Batch Finished");
        usleep(2000000);
    }
    return NULL;
}
