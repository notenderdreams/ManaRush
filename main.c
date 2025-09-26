#include "raylib.h"
#include "game.h"
#include <stdio.h>
#include <math.h>

int main(int argc, char **argv) {
    // Check command line arguments - require a map file
    if (argc < 2) { 
        printf("Usage: %s <map.tmx>\n", argv[0]); 
        return 1; 
    }

    // Initialize the game window
    InitWindow(DISPLAY_W, DISPLAY_H, "Mana Rush");
    SetTargetFPS(60);

    // Load menu background texture
    Texture2D menuBackground = LoadTexture("background.png");

    // Load custom font for menu text
    Font menuFont = LoadFont("The Golden Blade.ttf");

    // Initialize audio system and load background music
    InitAudioDevice(); // Initialize audio
    Music bgMusic = LoadMusicStream("bg_music.mp3");
    bgMusic.looping = true;      // set looping
    PlayMusicStream(bgMusic);    // start playing

    // Menu state variables
    bool showMenu = true;
    // Define button rectangles for menu
    Rectangle startButton = {DISPLAY_W/2 - 100, DISPLAY_H/2 - 25, 200, 50};
    Rectangle exitButton  = {DISPLAY_W/2 - 100, DISPLAY_H/2 + 50, 200, 50};

    // Game state structure
    GameState game;

    // Main game loop - runs until window is closed
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
                InitGame(&game, argv[1]); // Initialize game with map file
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

        // Begin drawing frame
        BeginDrawing();
        ClearBackground(BLACK);

        // Render menu or game based on current state
        if (showMenu) {
            // Draw menu background scaled to window size
            DrawTexturePro(
                menuBackground,
                (Rectangle){ 0, 0, (float)menuBackground.width, (float)menuBackground.height },
                (Rectangle){ 0, 0, (float)DISPLAY_W, (float)DISPLAY_H },
                (Vector2){ 0, 0 },
                0.0f,
                WHITE
            );
            
            // Draw game title using custom font
            const char *title = "MANA RUSH";
            Vector2 titlePos = { (DISPLAY_W - MeasureTextEx(menuFont, title, 60, 1).x) / 2, 150 };
            DrawTextEx(menuFont, title, titlePos, 60, 1, GOLD);

            // Draw START button with hover effect
            Vector2 mousePoint = GetMousePosition();
            Color btnColorStart = CheckCollisionPointRec(mousePoint, startButton) ? GREEN : DARKGREEN;
            DrawRectangleRec(startButton, btnColorStart);
            DrawRectangleLinesEx(startButton, 2, BLACK);

            const char *startText = "START";
            Vector2 startPos = { startButton.x + (startButton.width - MeasureTextEx(menuFont, startText, 30, 1).x) / 2,
                                 startButton.y + 15 };
            DrawTextEx(menuFont, startText, startPos, 30, 1, WHITE);

            // Draw EXIT button with hover effect
            Color btnColorExit = CheckCollisionPointRec(mousePoint, exitButton) ? RED : MAROON;
            DrawRectangleRec(exitButton, btnColorExit);
            DrawRectangleLinesEx(exitButton, 2, BLACK);

            const char *exitText = "EXIT";
            Vector2 exitPos = { exitButton.x + (exitButton.width - MeasureTextEx(menuFont, exitText, 30, 1).x) / 2,
                                exitButton.y + 10 };
            DrawTextEx(menuFont, exitText, exitPos, 30, 1, WHITE);

        } else {
            // Render the actual game
            RenderGame(&game);
        }

        EndDrawing(); // End drawing frame
    }

    // Clean up game resources if we exit while in game state
    if (!showMenu) {
        CleanupGame(&game);
    }

    // Unload all resources before closing
    UnloadTexture(menuBackground);
    UnloadFont(menuFont);
    StopMusicStream(bgMusic);
    UnloadMusicStream(bgMusic);
    CloseAudioDevice(); // Close audio system

    CloseWindow(); // Close the game window
    return 0;
}