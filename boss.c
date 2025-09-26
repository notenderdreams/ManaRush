#include "boss.h"
#include <raymath.h>
#include <stdlib.h>
#include <math.h>

// Initialize the boss with starting values and load resources
void InitBoss(Boss *boss, Vector2 position, tmx_map *map) {
    // Set position and remember spawn point for movement boundaries
    boss->position = position;
    boss->spawn = position;
    
    // Set boss stats - high health pool for challenging fight
    boss->health = 10000;
    boss->speed = 100;
    
    // Animation system - controls sprite frame updates
    boss->frame = 0;              // Start at first animation frame
    boss->frameTime = 0.15f;      // Each frame lasts 0.15 seconds (~6-7 FPS)
    boss->frameCounter = 0;       // Timer for advancing frames
    
    // Dash attack system - boss charges at player
    boss->dashCooldown = 3.0f;    // Can dash every 3 seconds
    boss->dashSpeed = 500.0f;     // Very fast movement during dash
    boss->isDashing = false;      // Not currently dashing
    
    // Death animation system - plays when boss is defeated
    boss->isDead = false;                // Boss starts alive
    boss->deathAnimPlaying = false;      // Death animation not active
    boss->deathFrame = 0;                // Start death anim at frame 0
    boss->deathFrameCounter = 0;         // Timer for death animation
    boss->deathFrameTime = 0.12f;        // Speed of death animation
    boss->deathFinished = false;         // Death anim hasn't completed
    
    // Movement and AI behavior
    boss->facingRight = false;           // Start facing left
    boss->zigzagTimer = 0.0f;            // Timer for zigzag movement pattern
    boss->zigzagAmplitude = 50.0f;       // How wide the zigzag movement is
    boss->zigzagFrequency = 2.0f;        // How fast the zigzag oscillates
    boss->teleportTimer = 0.0f;          // Timer for teleport ability
    boss->teleportCooldown = 2.0f;       // Cooldown between teleports
    boss->isFlying = false;              // Starts on ground

    // Load boss attack skill textures - different for left/right facing
    boss->skillRight = LoadTexture("skill1.png");  // Right-facing attack sprites
    boss->skillLeft = LoadTexture("skill2.png");   // Left-facing attack sprites
    boss->deathTex =LoadTexture("skill7.png");
}

// Update boss logic each frame - handles AI, movement, attacks, and phase changes
void UpdateBoss(Boss *boss, float delta, tmx_map *map) {
    // Check if boss has been defeated
    if (boss->health <= 0) {
        boss->speed = 0;         // Stop all movement
        boss->isDashing = false; // Cancel any active dash
        boss->isDead = true;     // Mark as dead
        return;                  // Skip further updates
    }

    // PHASE SYSTEM: Boss gets more aggressive as health decreases
    // This creates a multi-phase boss fight that gets harder over time
    
    // Phase 1: Health above 5000 - Basic movement pattern
    if (boss->health > 5000) {
        boss->phase = 1;
        boss->speed = 100;              // Normal speed
        boss->zigzagFrequency = 2.0f;   // Standard zigzag
        boss->isFlying = false;         // Ground-based movement
    } 
    // Phase 2: Health between 2000-5000 - Gains new abilities
    else if (boss->health > 2000) {
        boss->phase = 2;
        boss->speed = 120;              // Faster movement
        boss->zigzagFrequency = 2.0f;
        boss->isFlying = false;         // Still on ground
        boss->teleportCooldown = 4.0f;  // Can teleport every 4 seconds
    } 
    // Phase 3: Health below 2000 - Final aggressive phase
    else {
        boss->phase = 3;
        boss->speed = 200;              // Very fast movement
        boss->zigzagFrequency = 2.0f;
        boss->isFlying = true;          // Now flies in the air
        boss->teleportCooldown = 2.0f;  // Teleports more frequently
    }

    // Calculate boss collision dimensions based on texture size
    float bossWidth = (boss->skillRight.width / 6) * 1.8f;  // Width based on 6-frame animation
    float bossHeight = boss->skillRight.height * 1.8f;      // Height with scaling
    
    // Define movement boundaries relative to spawn point
    float leftBound = boss->spawn.x - 800.0f;   // Can move 800 units left of spawn
    float rightBound = boss->spawn.x + 600.0f;  // Can move 600 units right of spawn
    float topBound = boss->spawn.y - 50.0f;     // Normal vertical range
    float bottomBound = boss->spawn.y + 50.0f;

    // Phase 3 (flying) gets larger vertical movement range
    if (boss->phase == 3) {
        topBound = boss->spawn.y - 200.0f;  // Can fly 200 units higher
        bottomBound = boss->spawn.y;        // But not below spawn point
    }

    // Update all ability timers
    boss->dashTimer += delta;        // Track time since last dash
    boss->zigzagTimer += delta;      // Update zigzag oscillation
    boss->teleportTimer += delta;    // Track teleport cooldown

    // PHASE 3 BEHAVIOR: Flying boss with complex movement patterns
    if (boss->phase == 3) {
        // Teleport ability: 20% chance to teleport when cooldown is ready
        if (boss->teleportTimer >= boss->teleportCooldown) {
            if (rand() % 100 < 20) {  // 20% probability
                // Teleport to random position within movement boundaries
                boss->position.x = leftBound + (float)(rand() % (int)(rightBound - leftBound));
                boss->position.y = topBound + (float)(rand() % (int)(bottomBound - topBound));
                
                // Ensure boss doesn't fly too close to the ground
                float groundLevel = map->height * map->tile_height - bossHeight;
                if (boss->position.y < groundLevel - 200.0f) {
                    boss->position.y = groundLevel - 200.0f;  // Minimum flying height
                }
            }
            boss->teleportTimer = 0.0f;  // Reset teleport timer
        }

        // Dash attack: initiate dash when cooldown is ready
        if (!boss->isDashing && boss->dashTimer >= boss->dashCooldown) {
            boss->isDashing = true;  // Start dash attack
            boss->dashTimer = 0;     // Reset dash timer
        }

        // Handle dash movement
        if (boss->isDashing) {
            // Dash in the direction the boss is facing
            float dashDir = boss->facingRight ? 1 : -1;
            boss->position.x += dashDir * boss->dashSpeed * delta;
            
            // Dash lasts for 0.5 seconds
            if (boss->dashTimer > 0.5f) {
                boss->isDashing = false;  // End dash
                boss->dashTimer = 0;      // Reset timer
            }
        } 
        // Normal movement with zigzag pattern
        else {
            // Basic movement in facing direction
            float dir = boss->facingRight ? 1 : -1;
            boss->position.x += dir * boss->speed * delta;
            
            // Add horizontal zigzag movement using sine wave
            float zigzagOffsetX = boss->zigzagAmplitude * sinf(boss->zigzagTimer * boss->zigzagFrequency);
            boss->position.x += zigzagOffsetX;
            
            // Add vertical zigzag movement for flying effect (smaller amplitude)
            float zigzagOffsetY = boss->zigzagAmplitude * 0.2f * sinf(boss->zigzagTimer * boss->zigzagFrequency * 0.5f);
            boss->position.y += zigzagOffsetY;

            // Bounce off boundaries and change direction
            if (boss->position.x < leftBound) {
                boss->facingRight = true;   // Hit left wall, face right
                boss->position.x = leftBound;
            }
            if (boss->position.x > rightBound) {
                boss->facingRight = false;  // Hit right wall, face left
                boss->position.x = rightBound;
            }
        }

        // Constrain flying altitude - keep boss above ground but within bounds
        float groundLevel = map->height * map->tile_height - bossHeight;
        if (boss->position.y < groundLevel - 200.0f) {
            boss->position.y = groundLevel - 200.0f;  // Minimum height
        }
        if (boss->position.y > groundLevel) {
            boss->position.y = groundLevel;  // Maximum height (ground level)
        }
        
        // Clamp final position to ensure it stays within defined boundaries
        boss->position.x = Clamp(boss->position.x, leftBound, rightBound);
        boss->position.y = Clamp(boss->position.y, topBound, bottomBound);
    }
    // PHASE 2 BEHAVIOR: Ground boss with teleport and dash abilities
    else if (boss->phase == 2) {
        // Teleport to random ground position when cooldown ready
        if (boss->teleportTimer >= boss->teleportCooldown) {
            boss->position.x = leftBound + (float)(rand() % (int)(rightBound - leftBound));
            boss->position.y = map->height * map->tile_height - bossHeight;  // Always on ground
            boss->teleportTimer = 0.0f;
        }

        // Dash attack system (same logic as phase 3)
        if (!boss->isDashing && boss->dashTimer >= boss->dashCooldown) {
            boss->isDashing = true;
            boss->dashTimer = 0;
        }

        if (boss->isDashing) {
            float dashDir = boss->facingRight ? 1 : -1;
            boss->position.x += dashDir * boss->dashSpeed * delta;
            if (boss->dashTimer > 0.5f) {
                boss->isDashing = false;
                boss->dashTimer = 0;
            }
        } else {
            // Normal movement with horizontal zigzag only (no vertical movement)
            float dir = boss->facingRight ? 1 : -1;
            boss->position.x += dir * boss->speed * delta;
            float zigzagOffset = boss->zigzagAmplitude * sinf(boss->zigzagTimer * boss->zigzagFrequency);
            boss->position.x += zigzagOffset;

            // Boundary bouncing
            if (boss->position.x < leftBound) {
                boss->facingRight = true;
                boss->position.x = leftBound;
            }
            if (boss->position.x > rightBound) {
                boss->facingRight = false;
                boss->position.x = rightBound;
            }
        }

        // Constrain to ground movement only
        boss->position.x = Clamp(boss->position.x, leftBound, rightBound);
        boss->position.y = map->height * map->tile_height - bossHeight;  // Lock to ground level
    }
    // PHASE 1 BEHAVIOR: Simple ground movement with boundary bouncing
    else if (boss->phase == 1) {
        // Basic left-right movement
        float dir = boss->facingRight ? 1 : -1;
        boss->position.x += dir * boss->speed * delta;

        // Simple boundary detection and direction change
        if (boss->position.x < leftBound) {
            boss->facingRight = true;   // Hit left wall, turn right
            boss->position.x = leftBound;
        }
        if (boss->position.x > rightBound) {
            boss->facingRight = false;  // Hit right wall, turn left
            boss->position.x = rightBound;
        }

        // Keep boss constrained to movement area and on ground
        boss->position.x = Clamp(boss->position.x, leftBound, rightBound);
        boss->position.y = map->height * map->tile_height - bossHeight;  // Ground level
    }

    // UPDATE BOSS ANIMATION FRAMES
    boss->frameCounter += delta;
    // Advance to next frame when frame timer expires
    if (boss->frameCounter >= boss->frameTime) {
        boss->frameCounter = 0;
        boss->frame++;
        // Loop animation through 6 frames (0-5)
        if (boss->frame >= 6) boss->frame = 0;
    }
}

// Draw the boss with appropriate animation state
void DrawBoss(Boss *boss) {
    float scale = 1.8f;  // Scale factor for boss size

    // DEATH ANIMATION: Draw death sequence when boss is defeated
    if (boss->isDead) {
        // Start death animation if it hasn't started yet
        if (!boss->deathAnimPlaying && !boss->deathFinished) {
            boss->deathAnimPlaying = true;
            boss->deathFrame = 0;
            boss->deathFrameCounter = 0;
        }

        // Play death animation frames
        if (boss->deathAnimPlaying && !boss->deathFinished) {
            // Death texture is a sprite sheet with 10 columns and 2 rows (20 total frames)
            int cols = 10;
            int rows = 2;
            int frameWidth = boss->deathTex.width / cols;
            int frameHeight = boss->deathTex.height / rows;

            // Calculate current frame position in the sprite sheet
            int currentRow = boss->deathFrame / cols;  // Which row (0 or 1)
            int currentCol = boss->deathFrame % cols;  // Which column (0-9)

            // Source rectangle - portion of texture to draw
            Rectangle source = {
                currentCol * frameWidth,    // X position in texture
                currentRow * frameHeight,   // Y position in texture
                frameWidth,                 // Width of frame
                frameHeight                 // Height of frame
            };
            
            // Destination rectangle - where to draw on screen
            Rectangle dest = {
                boss->position.x,           // X position on screen
                boss->position.y,           // Y position on screen
                frameWidth * scale,         // Scaled width
                frameHeight * scale         // Scaled height
            };

            // Draw the current death animation frame
            DrawTexturePro(boss->deathTex, source, dest, (Vector2){0,0}, 0, WHITE);

            // Advance to next frame based on time
            boss->deathFrameCounter += GetFrameTime();
            if (boss->deathFrameCounter >= boss->deathFrameTime) {
                boss->deathFrameCounter = 0;
                boss->deathFrame++;
                
                // Check if death animation is complete (played all 20 frames)
                if (boss->deathFrame >= cols * rows) {
                    boss->deathAnimPlaying = false;
                    boss->deathFinished = true;  // Mark animation as complete
                }
            }
        }
        return;  // Don't draw normal boss when dead
    }

    // NORMAL BOSS DRAWING: Draw living boss with current animation frame
    
    // Calculate frame dimensions (texture has 6 frames horizontally)
    int frameWidth = boss->skillRight.width / 6;
    int frameHeight = boss->skillRight.height;
    
    // Source rectangle - select current frame from sprite sheet
    Rectangle source = { 
        boss->frame * frameWidth,  // X position based on current frame
        0,                         // Y position (top of texture)
        frameWidth,                // Width of one frame
        frameHeight                // Height of frame
    };
    
    // Destination rectangle - where to draw on screen
    Rectangle dest = { 
        boss->position.x,          // X position on screen
        boss->position.y,          // Y position on screen
        frameWidth * scale,        // Scaled width
        frameHeight * scale        // Scaled height
    };

    // Choose texture based on facing direction
    Texture2D current = boss->facingRight ? boss->skillRight : boss->skillLeft;
    
    // Draw the boss with the selected texture and current animation frame
    DrawTexturePro(current, source, dest, (Vector2){0,0}, 0, WHITE);
}

// Clean up boss resources - free loaded textures
void CleanupBoss(Boss *boss) {
    UnloadTexture(boss->skillRight);  // Free right-facing skill texture
    UnloadTexture(boss->skillLeft);   // Free left-facing skill texture
    UnloadTexture(boss->deathTex); 
}