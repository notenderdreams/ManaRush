#ifndef GAME_H
#define GAME_H

#include "raylib.h"      
#include "tmx.h"         
#include "character.h"   
#include "boss.h"        
#include "projectile.h"  

//========================
//      Constants
//========================
#define DISPLAY_W 1450      // width in pixels
#define DISPLAY_H 850        //  height in pixels
#define CAMERA_SPEED 300.0f  // Camera movement speed

// GameState
typedef struct {
    Camera2D camera;     // Camera
    tmx_map *map;        // map 
    Animation player;    // player
    Boss boss;          // Boss 
    float shootTimer;   // Timer for shooting cooldown
    bool wasColliding;  // Track previous collision state
    bool gameOver;      // Game over flag
    bool playerWon;     // victory flag
} GameState;

// Function declarations - game lifecycle management
void InitGame(GameState *game, const char *mapPath);  // Initialize game with map file
void UpdateGame(GameState *game, float delta);        // Update game logic each frame
void RenderGame(GameState *game);                     // Render/draw game graphics
void CleanupGame(GameState *game);                    // Clean up resources

#endif 