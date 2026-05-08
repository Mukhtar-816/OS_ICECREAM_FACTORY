#include "raylib.h"
#include "../include/factory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// Function prototypes from factory.c
void* mixer_thread(void* arg);
void* freezer_thread(void* arg);
void* packager_thread(void* arg);

// Function prototypes from gui.c
void DrawFactoryGUI(FactoryState state);

int main() {
    const int screenWidth = 1200;
    const int screenHeight = 950;

    InitWindow(screenWidth, screenHeight, "OS Ice Cream Factory - Management System");
    SetTargetFPS(60);

    // Initialize Semaphores
    sem_init(&sem_vats, 0, 3);      // 3 mixing vats
    sem_init(&sem_freezers, 0, 2);  // 2 freezers
    sem_init(&sem_packaging, 0, 2); // 2 packaging stations
    sem_init(&sem_ready_to_freeze, 0, 0); // Pipeline: Waiting for mixers
    sem_init(&sem_ready_to_pack, 0, 0);   // Pipeline: Waiting for freezers

    // Initialize State
    pthread_mutex_lock(&gui_mutex);
    global_state.num_workers = 15;
    global_state.available_vats = 3;
    global_state.available_freezers = 2;
    global_state.available_packaging_stations = 2;
    global_state.total_produced = 0;
    global_state.log_count = 0;
    global_state.start_time = GetTime();
    global_state.batches_per_minute = 0;
    for (int i = 0; i < 15; i++) {
        global_state.workers[i].id = i;
        global_state.workers[i].status = READY;
        global_state.workers[i].progress = 0;
        if (i < 5) strcpy((char*)global_state.workers[i].type, "MIXER");
        else if (i < 10) strcpy((char*)global_state.workers[i].type, "FREEZER");
        else strcpy((char*)global_state.workers[i].type, "PACKAGER");
    }
    pthread_mutex_unlock(&gui_mutex);

    // Spawn Worker Threads
    pthread_t threads[15];
    for (int i = 0; i < 15; i++) {
        int* id = malloc(sizeof(int));
        *id = i;
        if (i < 5) pthread_create(&threads[i], NULL, mixer_thread, id);
        else if (i < 10) pthread_create(&threads[i], NULL, freezer_thread, id);
        else pthread_create(&threads[i], NULL, packager_thread, id);
        pthread_detach(threads[i]);
    }

    add_log("Factory Started...");

    // Main Loop
    while (!WindowShouldClose()) {
        // --- STEP A: LOCK ---
        pthread_mutex_lock(&gui_mutex);
        
        // --- STEP B: SNAPSHOT (State Copy) ---
        FactoryState draw_state;
        memcpy(&draw_state, (void*)&global_state, sizeof(FactoryState));
        
        // --- STEP C: UNLOCK ---
        pthread_mutex_unlock(&gui_mutex);

        // --- STEP D: RENDER ---
        DrawFactoryGUI(draw_state);
    }

    // Cleanup
    CloseWindow();
    sem_destroy(&sem_vats);
    sem_destroy(&sem_freezers);
    sem_destroy(&sem_packaging);
    pthread_mutex_destroy(&gui_mutex);

    return 0;
}
