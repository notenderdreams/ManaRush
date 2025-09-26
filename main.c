#include "raylib.h"
#include "game.h"
#include <stdio.h>
#include <math.h>

int main(int argc, char **argv) {
    // For now its loading the map from commandline args . it shouldbe hardcoded
    if (argc < 2) { 
        printf("Usage: %s <map.tmx>\n", argv[0]); 
        return 1; 
    }

    // Initialize the game window
    InitWindow(DISPLAY_W, DISPLAY_H, "Mana Rush");
    SetTargetFPS(60);

    // Load menu 
    Texture2D menuBackground = LoadTexture("background.png");

    // Load custom font 
    Font menuFont = LoadFont("The Golden Blade.ttf");

    //========================
    //          Audio 
    //========================
    /*
        -init
        -load
        -enable looping 
        -start 
    */
    InitAudioDevice();              
    Music bgMusic = LoadMusicStream("bg_music.mp3");
    bgMusic.looping = true;
    PlayMusicStream(bgMusic);



    //========================
    //      Menu 
    //========================
    bool showMenu = true;
    // Buttons
    Rectangle startButton = {DISPLAY_W/2 - 100, DISPLAY_H/2 - 25, 200, 50};
    Rectangle exitButton  = {DISPLAY_W/2 - 100, DISPLAY_H/2 + 50, 200, 50};

    // Game state structure
    GameState game;

    // Main Loop
    while (!WindowShouldClose()) {
        // Update music stream (required for streaming music)
        UpdateMusicStream(bgMusic);

        // Handle menu state
        if (showMenu) {
            Vector2 mousePoint = GetMousePosition();

            // Check if start button is clicked
            if (CheckCollisionPointRec(mousePoint, startButton) && 
                IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                showMenu = false;
                InitGame(&game, argv[1]); 
            }

            // Check if exit button is clicked
            if (CheckCollisionPointRec(mousePoint, exitButton) &&
                IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                break; // exit game
            }

            // ESC key also exits from menu
            if (IsKeyPressed(KEY_ESCAPE)) {
                break;
            }
        } 
        // Handle game state (when not in menu)
        else {
            float dt = GetFrameTime(); // Get delta time for frame-rate independent movement
            UpdateGame(&game, dt); // Update game logic

            // ESC key returns to menu
            if (IsKeyPressed(KEY_ESCAPE)) {
                showMenu = true;
                CleanupGame(&game); // Clean up game resources
            }
        }

        // Begin
        BeginDrawing();
        ClearBackground(BLACK);

        if (showMenu) {
            // Draw menu background 
            DrawTexturePro(
                menuBackground,
                (Rectangle){ 0, 0, (float)menuBackground.width, (float)menuBackground.height },
                (Rectangle){ 0, 0, (float)DISPLAY_W, (float)DISPLAY_H },
                (Vector2){ 0, 0 },
                0.0f,
                WHITE
            );
            
            // Title
            const char *title = "MANA RUSH";
            Vector2 titlePos = { (DISPLAY_W - MeasureTextEx(menuFont, title, 60, 1).x) / 2, 150 };
            DrawTextEx(menuFont, title, titlePos, 60, 1, GOLD);

            // START Button
            Vector2 mousePoint = GetMousePosition();
            Color btnColorStart = CheckCollisionPointRec(mousePoint, startButton) ? GREEN : DARKGREEN;
            DrawRectangleRec(startButton, btnColorStart);
            DrawRectangleLinesEx(startButton, 2, BLACK);

            const char *startText = "START";
            Vector2 startPos = { startButton.x + (startButton.width - MeasureTextEx(menuFont, startText, 30, 1).x) / 2,
                                 startButton.y + 15 };
            DrawTextEx(menuFont, startText, startPos, 30, 1, WHITE);

            // EXIT Button
            Color btnColorExit = CheckCollisionPointRec(mousePoint, exitButton) ? RED : MAROON;
            DrawRectangleRec(exitButton, btnColorExit);
            DrawRectangleLinesEx(exitButton, 2, BLACK);

            const char *exitText = "EXIT";
            Vector2 exitPos = { exitButton.x + (exitButton.width - MeasureTextEx(menuFont, exitText, 30, 1).x) / 2,
                                exitButton.y + 10 };
            DrawTextEx(menuFont, exitText, exitPos, 30, 1, WHITE);

        } else {
            // Actual game
            RenderGame(&game);
        }

        EndDrawing();
    }

    // Clean up game resources if we exit while in game state
    if (!showMenu) {
        CleanupGame(&game);
    }

    //Cleanup
    UnloadTexture(menuBackground);
    UnloadFont(menuFont);
    StopMusicStream(bgMusic);
    UnloadMusicStream(bgMusic);
    CloseAudioDevice(); 

    CloseWindow(); 
    return 0;
}