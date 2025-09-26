#include "projectile.h"
#include "boss.h"
#include <stdlib.h>
#include <math.h>

// Global variables for projectile system
Projectile projectiles[MAX_PROJECTILES];  // Array to manage all projectiles
Texture2D fireTexture;                    // Texture for projectile graphics

// Load or generate the fire projectile texture
void LoadFireTexture() {
    // Try to load texture from file
    fireTexture = LoadTexture("fire_projectile.png");
    
    // If file loading fails (texture width is 0), generate a procedural fire texture
    if (fireTexture.width == 0) {
        // Create a blank 16x16 image
        Image fireImage = GenImageColor(16, 16, BLANK);
        
        // Generate a circular fire pattern programmatically
        for (int y = 0; y < 16; y++) {
            for (int x = 0; x < 16; x++) {
                // Calculate distance from center of image
                float dist = sqrtf((x-8)*(x-8) + (y-8)*(y-8));
                
                // Create a radial gradient: orange center, red outer ring
                if (dist < 8) {
                    Color fireColor = dist < 4 ? ORANGE : RED;  // Inner circle orange, outer ring red
                    ImageDrawPixel(&fireImage, x, y, fireColor);
                }
            }
        }
        
        // Convert the generated image to a texture
        fireTexture = LoadTextureFromImage(fireImage);
        UnloadImage(fireImage);  // Free the image data after creating texture
    }
}

// Unload the projectile texture from memory
void UnloadFireTexture() {
    UnloadTexture(fireTexture);
}

// Initialize all projectiles to inactive state
void InitProjectiles() {
    LoadFireTexture();  // Ensure texture is loaded
    
    // Initialize each projectile in the array
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        projectiles[i].active = false;        // Start inactive
        projectiles[i].scale = 1.0f;          // Default scale
        projectiles[i].rotation = 0.0f;       // No rotation
        projectiles[i].initialPosition = (Vector2){0, 0};  // Initialize starting position
    }
}

// Create and fire a new projectile from the character's position
void ShootProjectile(Animation *anim) {
    // Find an available (inactive) projectile slot
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!projectiles[i].active) {
            // Activate this projectile
            projectiles[i].active = true;
            projectiles[i].damage = PROJECTILE_DAMAGE;  // Set standard damage
            
            // Position projectile at character's hand/weapon position
            projectiles[i].position = (Vector2){
                anim->position.x + (anim->facingRight ? FRAME_WIDTH * anim->scale : 0),  // Right or left side
                anim->position.y + (FRAME_HEIGHT * anim->scale) / 2  // Middle of character height
            };
            
            projectiles[i].initialPosition = projectiles[i].position;  // Store starting point for range calculation
            
            // Set velocity based on character's facing direction
            projectiles[i].velocity = (Vector2){
                anim->facingRight ? PROJECTILE_SPEED : -PROJECTILE_SPEED,  // Right or left movement
                0  // No vertical movement initially
            };
            
            projectiles[i].scale = 1.5f;     // Slightly larger scale
            projectiles[i].rotation = 0.0f;  // Start with no rotation
            break;  // Only create one projectile per shot
        }
    }
}

// Update all active projectiles - movement, collision, effects
void UpdateProjectiles(float delta, tmx_map *map, Boss *boss) {
    // Calculate map boundaries for collision detection
    float map_w = map->width * map->tile_width;
    float map_h = map->height * map->tile_height;
    
    // Process each projectile in the array
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active) {
            // BASIC MOVEMENT: Apply velocity to position
            projectiles[i].position.x += projectiles[i].velocity.x * delta;
            projectiles[i].position.y += projectiles[i].velocity.y * delta;
            
            // FIRE EFFECT: Add subtle vertical wobble using sine wave
            projectiles[i].position.y += sinf(GetTime() * 10.0f + i) * 0.5f;
            
            // ROTATION EFFECT: Spin projectile based on direction
            projectiles[i].rotation += (projectiles[i].velocity.x > 0 ? 10.0f : -10.0f) * delta;
            
            // PULSATING SCALE: Make fireball pulse in size
            projectiles[i].scale = 1.5f + sinf(GetTime() * 15.0f + i) * 0.3f;

            // BOSS COLLISION DETECTION
            if (!boss->isDead) {  // Only check collision if boss is alive
                // Calculate boss collision rectangle
                float bossW = (boss->skillRight.width / 6) * 1.8f;
                float bossH = boss->skillRight.height * 1.8f;
                Rectangle bossRect = { 
                    boss->position.x, 
                    boss->position.y, 
                    bossW, 
                    bossH 
                };
                
                // Calculate projectile collision rectangle (scaled texture size)
                float projSize = fireTexture.width * projectiles[i].scale * 0.5f;
                Rectangle projRect = { 
                    projectiles[i].position.x - projSize/2,  // Center the collision box
                    projectiles[i].position.y - projSize/2, 
                    projSize, 
                    projSize 
                };
                
                // Check if projectile collides with boss
                if (CheckCollisionRecs(bossRect, projRect)) {
                    projectiles[i].active = false;  // Deactivate projectile
                    boss->health -= projectiles[i].damage;  // Apply damage to boss
                    if (boss->health < 0) boss->health = 0;  // Clamp health at zero
                }
            }

            // MAP BOUNDARY COLLISION: Deactivate if outside map
            if (projectiles[i].position.x < 0 || projectiles[i].position.x > map_w ||
                projectiles[i].position.y < 0 || projectiles[i].position.y > map_h) {
                projectiles[i].active = false;
            }
            
            // RANGE LIMIT: Auto-despawn after traveling 1000 pixels
            float travelDistance = fabsf(projectiles[i].position.x - projectiles[i].initialPosition.x);
            if (travelDistance > 1000.0f) {
                projectiles[i].active = false;
            }
        }
    }
}

// Draw all active projectiles with visual effects
void DrawProjectiles() {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active) {
            // Set up texture drawing parameters
            Vector2 origin = { fireTexture.width/2, fireTexture.height/2 };  // Rotate around center
            Rectangle source = { 0, 0, fireTexture.width, fireTexture.height };  // Full texture
            Rectangle dest = {
                projectiles[i].position.x,
                projectiles[i].position.y,
                fireTexture.width * projectiles[i].scale,   // Scaled width
                fireTexture.height * projectiles[i].scale   // Scaled height
            };
            
            // Flip texture based on movement direction (for consistency)
            if (projectiles[i].velocity.x < 0) {
                source.width = -source.width;  // Flip horizontally
            }
            
            // Draw main fire texture with orange tint
            Color fireTint = (Color){255, 165, 0, 255}; // Orange color
            DrawTexturePro(fireTexture, source, dest, origin, projectiles[i].rotation, fireTint);
            
            // Add glowing overlay effect (semi-transparent yellow)
            DrawTexturePro(fireTexture, source, dest, origin, projectiles[i].rotation, 
                          Fade(YELLOW, 0.3f + sinf(GetTime() * 20.0f) * 0.2f));  // Pulsating alpha
        }
    }
}

// Clean up projectile system resources
void CleanupProjectiles() {
    UnloadFireTexture();  // Free the texture memory
}