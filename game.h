#ifndef GAME_H
#define GAME_H

// Include necessary libraries
#include "raylib.h"      // Raylib graphics/audio/input library
#include "tmx.h"         // TMX map loader for tilemaps
#include "character.h"   // Character/player definitions
#include "boss.h"        // Boss enemy definitions
#include "projectile.h"  // Projectile/bullet definitions

// Screen dimensions constants
#define DISPLAY_W 1450   // Window width in pixels
#define DISPLAY_H 850    // Window height in pixels
#define CAMERA_SPEED 300.0f  // Camera movement speed

// Main game state structure - holds all game data
typedef struct {
    Camera2D camera;     // 2D camera for viewport control
    tmx_map *map;        // Loaded tilemap from TMX file
    Animation player;    // Player character with animation
    Boss boss;          // Boss enemy
    float shootTimer;   // Timer for shooting cooldown
    bool wasColliding;  // Track previous collision state
    bool gameOver;      // Game over flag
    bool playerWon;     // Player victory flag
} GameState;

// Function declarations - game lifecycle management
void InitGame(GameState *game, const char *mapPath);  // Initialize game with map file
void UpdateGame(GameState *game, float delta);        // Update game logic each frame
void RenderGame(GameState *game);                     // Render/draw game graphics
void CleanupGame(GameState *game);                    // Clean up resources

#endif 