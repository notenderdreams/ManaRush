#include "character.h"
#include "projectile.h"
#include <stdlib.h>
#include <string.h>

#define SHOOT_COOLDOWN 0.5f  // Time between shots in seconds

// Helper function to check if two rectangles intersect
static bool RectIntersects(Rectangle a, Rectangle b) {
    return !(a.x + a.width <= b.x || b.x + b.width <= a.x ||
             a.y + a.height <= b.y || b.y + b.height <= a.y);
}

// Helper function to identify which tile layers are collidable
static bool IsCollidableTileLayer(const char *name) {
    return (strcmp(name, "platform") == 0 ||
            strcmp(name, "bridge") == 0 ||
            strcmp(name, "uw_plant") == 0);
}

// Check for collisions with tiles in the map
static Rectangle* FindTileCollision(tmx_map *map, Rectangle char_rect) {
    tmx_layer *layer = map->ly_head;
    // Iterate through all layers in the map
    while (layer) {
        // Check if layer is visible and is a tile layer with collidable name
        if (layer->visible && layer->type == L_LAYER && IsCollidableTileLayer(layer->name)) {
            // Scan through all tiles in this layer
            for (int i = 0; i < map->height; i++) {
                for (int j = 0; j < map->width; j++) {
                    // Get tile ID (remove flip flags)
                    unsigned int gid = (layer->content.gids[(i*map->width)+j]) & TMX_FLIP_BITS_REMOVAL;
                    if (gid) { // If tile exists at this position
                        // Create rectangle for this tile
                        Rectangle tileRect = {
                            j * map->tile_width,
                            i * map->tile_height,
                            (float)map->tile_width,
                            (float)map->tile_height
                        };
                        // Check collision with character
                        if (RectIntersects(char_rect, tileRect)) {
                            Rectangle *hit = malloc(sizeof(Rectangle));
                            *hit = tileRect;
                            return hit; // Return colliding tile
                        }
                    }
                }
            }
        }
        layer = layer->next;
    }
    return NULL; // No collision found
}

// Initialize the character animation and state
void InitAnimation(Animation* anim, float tile_height) {
    // Load character sprite sheet
    anim->spriteSheet = LoadTexture("main_character.png");
    anim->position = (Vector2){0, 0};
    anim->facingRight = true;
    // Scale character relative to tile height for consistent sizing
    anim->scale = (3.0f * tile_height) / FRAME_HEIGHT;
    anim->health = 2000;
    anim->isPaused = false;

    // Define number of frames for each animation row
    int frameCounts[] = {4, 6, 10, 9, 10, 10, 7, 5};
    // Set up frame rectangles for sprite sheet
    for (int row = 0; row < NUM_ROWS; row++) {
        anim->frameCount[row] = frameCounts[row];
        for (int col = 0; col < frameCounts[row]; col++) {
            anim->frames[row][col] = (Rectangle){
                col * FRAME_WIDTH, row * FRAME_HEIGHT,
                FRAME_WIDTH, FRAME_HEIGHT
            };
        }
    }
    
    // Initialize animation state
    anim->currentRow = 0;
    anim->currentFrame = 0;
    anim->frameTimer = 0.0f;
    anim->isLooping = true;
    anim->isPlayingOneShot = false;
    anim->velocity = (Vector2){0, 0};
    anim->grounded = false;
}

// Update animation frames based on elapsed time
void UpdateAnimation(Animation* anim, float delta) {
    anim->frameTimer += delta;
    // Advance frame every 0.1 seconds (10 FPS animation)
    if (anim->frameTimer >= 0.1f) {
        anim->frameTimer = 0.0f;
        anim->currentFrame++;
        
        // Handle end of animation sequence
        if (anim->currentFrame >= anim->frameCount[anim->currentRow]) {
            if (anim->isLooping) {
                // Loop back to first frame
                anim->currentFrame = 0;
            } else {
                // Stop at last frame for one-shot animations
                anim->currentFrame = anim->frameCount[anim->currentRow] - 1;
                anim->isPlayingOneShot = false;
                anim->isLooping = true;
                // Return to idle animation
                anim->currentRow = 0;
                anim->currentFrame = 0;
            }
        }
    }
}

// Play a specific animation sequence
void PlayAnimation(Animation* anim, int row, bool loop) {
    // Don't interrupt one-shot animations
    if (anim->isPlayingOneShot) return;
    
    // Only change if different animation or switching from one-shot to loop
    if (anim->currentRow != row || !anim->isLooping) {
        anim->currentRow = row;
        anim->currentFrame = 0;
        anim->isLooping = loop;
        anim->isPlayingOneShot = !loop; // Mark as one-shot if not looping
    }
}

// Draw the character to the screen
void DrawAnimation(Animation* anim) {
    // Ensure frame index is valid
    int frame = anim->currentFrame;
    if (frame >= anim->frameCount[anim->currentRow]) {
        frame = anim->frameCount[anim->currentRow] - 1;
    }
    
    // Set up source and destination rectangles
    Rectangle src = anim->frames[anim->currentRow][frame];
    Rectangle dest = {
        anim->position.x,
        anim->position.y,
        FRAME_WIDTH * anim->scale,
        FRAME_HEIGHT * anim->scale
    };
    Vector2 origin = {0, 0};
    
    // Flip sprite if facing left
    if (!anim->facingRight) src.width = -src.width;
    
    DrawTexturePro(anim->spriteSheet, src, dest, origin, 0.0f, WHITE);
}

// Main character update function - handles input, physics, and collisions
void UpdateCharacter(Animation* anim, float delta, tmx_map *map, float *shootTimer) {
    // Skip update if character is paused (game over, etc.)
    if (anim->isPaused) {
        anim->velocity.x = 0;
        return;
    }
    
    // Handle animation based on input and state
    if (!anim->isPlayingOneShot) {
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) 
            PlayAnimation(anim, 1, true); // Run animation
        else if (IsKeyPressed(KEY_SPACE) && anim->grounded) 
            PlayAnimation(anim, 2, false); // Jump animation (one-shot)
        else if (anim->currentRow != 0) 
            PlayAnimation(anim, 0, true); // Idle animation
    }

    // Handle horizontal movement input
    float move_x = 0.0f;
    if (IsKeyDown(KEY_A)) { 
        anim->facingRight = false; 
        move_x -= MOVE_SPEED; 
    }
    if (IsKeyDown(KEY_D)) { 
        anim->facingRight = true; 
        move_x += MOVE_SPEED; 
    }

    // Handle jumping
    if (IsKeyPressed(KEY_SPACE) && anim->grounded) {
        anim->velocity.y = JUMP_VELOCITY;
        anim->grounded = false;
    }

    // Handle shooting with cooldown
    *shootTimer -= delta;
    if (IsKeyPressed(KEY_E) && *shootTimer <= 0.0f) {
        ShootProjectile(anim);
        *shootTimer = SHOOT_COOLDOWN;
    }

    // Apply gravity and clamp fall speed
    anim->velocity.y += GRAVITY * delta;
    if (anim->velocity.y > MAX_FALL_SPEED) 
        anim->velocity.y = MAX_FALL_SPEED;
    anim->velocity.x = move_x;

    // Calculate character collision rectangle
    float char_w = FRAME_WIDTH * anim->scale;
    float char_h = FRAME_HEIGHT * anim->scale;
    Rectangle char_rect = {anim->position.x, anim->position.y, char_w, char_h};

    // X-axis collision detection and response
    Rectangle new_x = char_rect;
    new_x.x += anim->velocity.x * delta;
    Rectangle *coll_x = FindTileCollision(map, new_x);
    if (!coll_x) {
        // No collision - apply movement
        anim->position.x = new_x.x;
    } else {
        // Collision detected - adjust position based on direction
        if (anim->velocity.x > 0) 
            anim->position.x = coll_x->x - char_w - 0.1f; // Stop at left side of tile
        else 
            anim->position.x = coll_x->x + coll_x->width + 0.1f; // Stop at right side of tile
        anim->velocity.x = 0; // Stop horizontal movement
        free(coll_x);
    }
    char_rect.x = anim->position.x; // Update rectangle position

    // Y-axis collision detection and response
    Rectangle new_y = char_rect;
    new_y.y += anim->velocity.y * delta;
    Rectangle *coll_y = FindTileCollision(map, new_y);
    if (!coll_y) {
        // No collision - apply movement
        anim->position.y = new_y.y;
        anim->grounded = false;
    } else {
        if (anim->velocity.y > 0) {
            // Landing on ground
            anim->position.y = coll_y->y - char_h;
            anim->grounded = true;
        } else {
            // Hitting ceiling
            anim->position.y = coll_y->y + coll_y->height + 0.1f;
            anim->velocity.y = 0;
        }
        free(coll_y);
    }

    // Keep character within map boundaries
    float map_w = map->width * map->tile_width;
    float map_h = map->height * map->tile_height;
    if (anim->position.x < 0) anim->position.x = 0;
    if (anim->position.x > map_w - char_w) anim->position.x = map_w - char_w;

    if (anim->position.y < 0) {
        anim->position.y = 0;
        anim->velocity.y = 0;
    }
    if (anim->position.y > map_h - char_h) {
        anim->position.y = map_h - char_h;
        anim->grounded = true;
        anim->velocity.y = 0;
    }

    // Check if character is standing on solid ground (prevent falling through)
    if (anim->grounded) {
        Rectangle feet_rect = {anim->position.x, anim->position.y + char_h - 1, char_w, 2};
        Rectangle *coll_feet = FindTileCollision(map, feet_rect);
        if (!coll_feet) {
            // Character is floating - game over
            anim->health = 0;
            anim->isPaused = true;
        } else {
            free(coll_feet);
        }
    }

    // Update animation frames
    UpdateAnimation(anim, delta);
}

// Clean up character resources
void CleanupAnimation(Animation* anim) {
    UnloadTexture(anim->spriteSheet);
}