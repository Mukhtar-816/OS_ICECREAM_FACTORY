#include "raylib.h"
#include "../include/factory.h"
#include <stdio.h>
#include <string.h>

// --- INSTRUCTOR DASHBOARD THEME ---
#define COLOR_APP_BG      (Color){ 245, 245, 250, 255 }
#define COLOR_PANEL       WHITE
#define COLOR_HEADER      (Color){ 52, 73, 94, 255 }
#define COLOR_TEXT        (Color){ 44, 62, 80, 255 }
#define COLOR_BORDER      (Color){ 189, 195, 199, 255 }

// OS State Colors
#define COLOR_READY       (Color){ 149, 165, 166, 255 } // Gray
#define COLOR_RUNNING     (Color){ 39, 174, 96, 255 }   // Green
#define COLOR_WAITING     (Color){ 192, 57, 43, 255 }   // Red
#define COLOR_DISPATCH    (Color){ 241, 196, 15, 255 }  // Yellow/Gold

void DrawDiagnosticPanel(Rectangle rec, const char* title) {
    DrawRectangleRec(rec, COLOR_PANEL);
    DrawRectangleLinesEx(rec, 1, COLOR_BORDER);
    DrawRectangle(rec.x, rec.y, rec.width, 35, COLOR_HEADER);
    DrawText(title, rec.x + 15, rec.y + 8, 20, WHITE);
}

void DrawWorkerFlow(FactoryState state, const char* type, int x, int y, int width, int height) {
    Rectangle rec = { (float)x, (float)y, (float)width, (float)height };
    DrawDiagnosticPanel(rec, type);
    
    int count = 0;
    for (int i = 0; i < state.num_workers; i++) {
        if (strcmp(state.workers[i].type, type) == 0) {
            int yOff = y + 50 + (count * 45);
            Color statusColor = COLOR_READY;
            const char* stateLabel = "READY";
            
            if (state.workers[i].status == RUNNING) {
                statusColor = COLOR_RUNNING;
                stateLabel = "RUNNING";
            } else if (state.workers[i].status == WAITING) {
                statusColor = COLOR_WAITING;
                stateLabel = "WAITING";
            } else if (state.workers[i].status == DISPATCHING) {
                statusColor = COLOR_DISPATCH;
                stateLabel = "DISPATCH";
            }

            DrawText(TextFormat("[%02d] %s", state.workers[i].id, stateLabel), x + 15, yOff, 16, statusColor);
            
            // Progress Track
            DrawRectangle(x + 140, yOff, 200, 15, (Color){ 230, 230, 235, 255 });
            DrawRectangle(x + 140, yOff, (int)(state.workers[i].progress * 2.0f), 15, statusColor);
            
            DrawText(TextFormat("-> %s", state.workers[i].holding_resource), x + 140, yOff + 18, 12, DARKGRAY);
            
            count++;
        }
    }
}

void DrawFactoryGUI(FactoryState state) {
    BeginDrawing();
    ClearBackground(COLOR_APP_BG);

    // --- WORKFLOW VISUALIZATION (Top Row) ---
    int flowWidth = 370;
    int flowHeight = 420;
    DrawWorkerFlow(state, "MIXER", 20, 20, flowWidth, flowHeight);
    DrawWorkerFlow(state, "FREEZER", 415, 20, flowWidth, flowHeight);
    DrawWorkerFlow(state, "PACKAGER", 810, 20, flowWidth, flowHeight);

    // Arrows
    DrawText(">>>", 390, 200, 20, LIGHTGRAY);
    DrawText(">>>", 785, 200, 20, LIGHTGRAY);

    // --- RESOURCE MONITOR (Middle Row) ---
    Rectangle semRec = { 20, 460, 580, 180 };
    DrawDiagnosticPanel(semRec, "OS RESOURCE MONITOR (SEMAPHORES)");
    
    int sx = semRec.x + 25;
    int sy = semRec.y + 55;
    
    int vatsInUse = 3 - state.available_vats;
    DrawText(TextFormat("Vats In Use:    %d / 3", vatsInUse), sx, sy, 20, COLOR_TEXT);
    for (int i = 0; i < 3; i++) {
        DrawCircle(sx + 450 + (i * 40), sy + 10, 12, (i < vatsInUse) ? COLOR_RUNNING : COLOR_READY);
    }

    int freezersInUse = 2 - state.available_freezers;
    DrawText(TextFormat("Freezers In Use: %d / 2", freezersInUse), sx, sy + 45, 20, COLOR_TEXT);
    for (int i = 0; i < 2; i++) {
        DrawRectangle(sx + 450 + (i * 50), sy + 43, 40, 20, (i < freezersInUse) ? COLOR_RUNNING : COLOR_READY);
    }

    int packInUse = 2 - state.available_packaging_stations;
    DrawText(TextFormat("Pack In Use:     %d / 2", packInUse), sx, sy + 90, 20, COLOR_TEXT);
    for (int i = 0; i < 2; i++) {
        DrawRectangle(sx + 450 + (i * 50), sy + 88, 40, 20, (i < packInUse) ? COLOR_RUNNING : COLOR_READY);
    }

    // --- STATUS LEGEND (Beside Resources) ---
    Rectangle legRec = { 620, 460, 280, 180 };
    DrawDiagnosticPanel(legRec, "STATUS LEGEND");
    DrawCircle(legRec.x + 20, legRec.y + 60, 8, COLOR_READY);    DrawText("READY", legRec.x + 45, legRec.y + 52, 16, DARKGRAY);
    DrawCircle(legRec.x + 150, legRec.y + 60, 8, COLOR_RUNNING); DrawText("RUNNING", legRec.x + 175, legRec.y + 52, 16, DARKGRAY);
    DrawCircle(legRec.x + 20, legRec.y + 100, 8, COLOR_WAITING);  DrawText("WAITING", legRec.x + 45, legRec.y + 92, 16, DARKGRAY);
    DrawCircle(legRec.x + 150, legRec.y + 100, 8, COLOR_DISPATCH);DrawText("DISPATCH", legRec.x + 175, legRec.y + 92, 16, DARKGRAY);

    // --- DASHBOARD (Beside Legend) ---
    Rectangle dashRec = { 920, 460, 260, 180 };
    DrawDiagnosticPanel(dashRec, "DASHBOARD");
    DrawText("Total Production", dashRec.x + 20, dashRec.y + 60, 18, DARKGRAY);
    DrawText(TextFormat("%d Units", state.total_produced), dashRec.x + 20, dashRec.y + 90, 30, COLOR_RUNNING);
    DrawText(TextFormat("Efficiency: %.1f BPM", state.batches_per_minute), dashRec.x + 20, dashRec.y + 140, 18, COLOR_TEXT);

    // --- NARRATIVE AUDIT LOG (Bottom Row) ---
    Rectangle logRec = { 20, 660, 1160, 270 };
    DrawDiagnosticPanel(logRec, "NARRATIVE DIAGNOSTIC AUDIT LOG");
    
    // Show more logs (up to 15)
    int maxDisplayLogs = 15;
    int displayStart = (state.log_count > maxDisplayLogs) ? (state.log_count - maxDisplayLogs) : 0;
    int currentLine = 0;
    
    for (int i = displayStart; i < state.log_count; i++) {
        DrawText(TextFormat("> %s", state.logs[i]), logRec.x + 20, logRec.y + 45 + (currentLine * 15), 12, DARKGRAY);
        currentLine++;
    }

    EndDrawing();
}
