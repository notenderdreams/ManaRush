#ifndef PROJECTILE_H
#define PROJECTILE_H

// Include necessary libraries for projectile functionality
#include "raylib.h"      // Graphics, audio, and input functionality
#include "tmx.h"         // Tilemap loading and collision detection
#include "character.h"   // Character/player definitions (for shooting)
#include "boss.h"        // Boss enemy definitions (for dealing damage)

// Projectile system constants
#define MAX_PROJECTILES 50       // Maximum number of active projectiles at once
#define PROJECTILE_SPEED 300.0f  // Base speed of projectiles in pixels per second
#define PROJECTILE_DAMAGE 200    // Damage dealt by each projectile to boss

// Projectile structure - represents a single fired projectile
typedef struct Projectile {
    bool active;           // Whether this projectile is currently active and moving
    Vector2 position;      // Current world coordinates of the projectile
    Vector2 velocity;      // Movement direction and speed (x, y components)
    Vector2 initialPosition;  // Starting position (useful for range calculations or effects)
    float damage;          // Damage this projectile deals (can vary per projectile)
    float scale;           // Size multiplier for drawing the projectile
    float rotation;        // Rotation angle for projectile sprite (for directional effects)
} Projectile;

// Global variables for projectile system
extern Projectile projectiles[MAX_PROJECTILES];  // Array storing all projectiles
extern Texture2D fireTexture;                    // Texture used for projectile graphics

// Function declarations - projectile system management

// Texture management functions
void LoadFireTexture();     // Loads the projectile texture from file
void UnloadFireTexture();   // Unloads the projectile texture from memory

// Projectile system lifecycle functions
void InitProjectiles();     // Initializes all projectiles to inactive state
void ShootProjectile(Animation *anim);  // Creates a new projectile from character position
void UpdateProjectiles(float delta, tmx_map *map, Boss *boss);  // Updates all active projectiles
void DrawProjectiles();     // Renders all active projectiles to the screen
void CleanupProjectiles();  // Cleans up all projectile resources

#endif