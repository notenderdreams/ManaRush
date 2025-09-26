#ifndef BOSS_H
#define BOSS_H

// Include necessary libraries for boss functionality
#include "raylib.h"      // Provides graphics, audio, and input functionality
#include "raymath.h"     // Provides vector math utilities (Vector2 operations, etc.)
#include "tmx.h"         // Provides tilemap loading and collision detection

// Boss structure definition - contains all data needed for boss behavior and rendering
typedef struct Boss {
    // Position and spawning information
    Vector2 position;        // Current world coordinates of the boss
    Vector2 spawn;           // Original spawn location for resetting or reference
    int health;              // Hit points - when <= 0, boss is defeated
    float speed;             // Base movement speed of the boss
    
    // Animation system for normal boss movements/attacks
    int frame;               // Current frame index in the animation sequence
    float frameTime;         // Time accumulated for frame updates
    float frameCounter;      // Counter to track when to advance to next frame
    
    // Attack skill textures (direction-specific)
    Texture2D skillRight;    // Texture used for right-facing attacks or skills
    Texture2D skillLeft;     // Texture used for left-facing attacks or skills
    
    // Battle phase management (for multi-phase boss fights)
    int phase;               // Current phase of the boss fight (1, 2, 3, etc.)
    bool facingRight;        // Direction the boss is facing (true=right, false=left)
    
    // Dash attack system
    bool isDashing;          // Flag indicating if boss is currently performing a dash attack
    float dashTimer;         // Tracks how long the dash has been active
    float dashCooldown;      // Time required before boss can dash again
    float dashSpeed;         // Special speed used during dash attacks (usually faster)
    
    // Zigzag movement pattern (for evasive or unpredictable movement)
    float zigzagTimer;       // Timer for controlling zigzag oscillation
    float zigzagAmplitude;   // How wide the zigzag movement swings (left-right distance)
    float zigzagFrequency;   // How quickly the zigzag pattern oscillates
    
    // Teleportation ability
    float teleportTimer;     // Timer tracking when to teleport next
    float teleportCooldown;  // Delay between teleportation abilities
    
    // Special state flags
    bool isFlying;           // Indicates if boss is in flying mode (different movement rules)
    bool isDead;             // Flag set when boss health reaches zero
    
    // Death animation system (plays when boss is defeated)
    bool deathAnimPlaying;   // Indicates if death animation is currently active
    int deathFrame;          // Current frame in the death animation sequence
    float deathFrameCounter; // Timer for death animation frame progression
    float deathFrameTime;    // Duration each death animation frame should display
    bool deathFinished;      // Flag set when death animation completes
} Boss;

// Function declarations:

// Initializes boss with starting values and loads resources
// Parameters: boss pointer, starting position, and map for collision data
void InitBoss(Boss *boss, Vector2 position, tmx_map *map);

// Updates boss logic each frame including AI, movement, attacks, and state changes
// Parameters: boss pointer, time since last frame, map for collision detection
void UpdateBoss(Boss *boss, float delta, tmx_map *map);

// Renders the boss to the screen with appropriate animation state
// Parameters: boss pointer, texture to use for death animation
void DrawBoss(Boss *boss, Texture2D deathTex);

// Cleans up and frees all resources allocated for the boss
// Parameters: boss pointer to clean up
void CleanupBoss(Boss *boss);

#endif