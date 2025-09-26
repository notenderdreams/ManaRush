#ifndef CHARACTER_H
#define CHARACTER_H

// Include necessary libraries
#include "raylib.h"  // Raylib graphics/audio/input library
#include "tmx.h"     // TMX map loader for collision detection

// Animation and character constants
#define FRAME_WIDTH 50      // Width of each animation frame in pixels
#define FRAME_HEIGHT 37     // Height of each animation frame in pixels
#define NUM_ROWS 8          // Number of animation rows in the sprite sheet
#define MAX_FRAMES 10       // Maximum number of frames per animation row

// Physics and movement constants
#define MOVE_SPEED 200.0f       // Character horizontal movement speed
#define GRAVITY 1200.0f         // Gravity force applied when falling
#define JUMP_VELOCITY -720.0f   // Initial upward velocity when jumping
#define MAX_FALL_SPEED 1000.0f  // Maximum falling speed to prevent excessive velocity

// Animation structure - holds all data for character animation and state
typedef struct Animation {
    // Sprite and animation data
    Texture2D spriteSheet;                  // Loaded sprite sheet texture
    Rectangle frames[NUM_ROWS][MAX_FRAMES]; // Array of frame rectangles for each animation row
    int currentRow;                         // Current animation row (type of animation)
    int currentFrame;                       // Current frame within the row
    int frameCount[NUM_ROWS];               // Number of frames in each row
    float frameTimer;                       // Timer for frame updates
    bool isLooping;                         // Whether animation should loop
    bool isPlayingOneShot;                  // Whether playing a one-time animation
    
    // Character state and physics
    Vector2 position;       // Current position in world coordinates
    bool facingRight;       // Character facing direction (true = right, false = left)
    float scale;           // Scale factor for drawing character
    Vector2 velocity;      // Current velocity (x, y)
    bool grounded;         // Whether character is touching the ground
    int health;           // Character health points
    bool isPaused;        // Whether character updates are paused (for game over states)
} Animation;

// Function declarations - character management
void InitAnimation(Animation* anim, float tile_height);           // Initialize character with animation
void UpdateCharacter(Animation* anim, float delta, tmx_map *map, float *shootTimer); // Update character logic
void UpdateAnimation(Animation* anim, float delta);               // Update animation frames
void PlayAnimation(Animation* anim, int row, bool loop);          // Play specific animation
void DrawAnimation(Animation* anim);                              // Draw character to screen
void CleanupAnimation(Animation* anim);                           // Clean up character resources

#endif