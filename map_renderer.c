#include "map_renderer.h"
#include "projectile.h" 
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// TMX texture loader callback - integrates TMX with Raylib's texture system
void *raylib_tex_loader(const char *path) {
    // Allocate memory for a Texture2D pointer
    Texture2D *returnValue = malloc(sizeof(Texture2D));
    // Load the texture using Raylib's LoadTexture function
    *returnValue = LoadTexture(path);
    return returnValue; // Return pointer to the loaded texture
}

// TMX texture deallocator callback - properly frees Raylib textures
void raylib_free_tex(void *ptr) {
    // Unload the texture from GPU memory using Raylib's UnloadTexture
    UnloadTexture(*((Texture2D *) ptr));
    // Free the memory allocated for the Texture2D pointer
    free(ptr);
}

// Convert integer color (from TMX) to Raylib Color structure
Color int_to_color(int color) {
    // Convert TMX color integer to RGBA bytes
    tmx_col_bytes res = tmx_col_to_bytes(color);
    // Cast the byte structure to Raylib's Color structure
    return *((Color*)&res);
}

// Draw a single tile from a tileset texture onto the screen
static void draw_tile(void *image, unsigned int sx, unsigned int sy, unsigned int sw, unsigned int sh, 
                     unsigned int dx, unsigned int dy, float opacity, unsigned int flags) {
    Texture2D *texture = (Texture2D*) image; // Cast the image pointer to Texture2D
    int op = 0xFF * opacity; // Calculate alpha value from opacity (0-1 to 0-255)
    
    // Draw the specific tile region from the tileset texture
    DrawTextureRec(*texture, 
                  (Rectangle){sx, sy, sw, sh}, // Source rectangle (which part of texture to draw)
                  (Vector2){dx, dy},           // Destination position (where to draw on screen)
                  (Color){op, op, op, op});    // Color with applied opacity (grayscale alpha)
}

// Draw a single layer of the tilemap
static void draw_layer(tmx_map *map, tmx_layer *layer) {
    unsigned long i, j;
    unsigned int gid, x, y, w, h, flags;
    float op = layer->opacity; // Get layer opacity for transparency effects
    
    tmx_tileset *ts;    // Pointer to tileset data
    tmx_image *im;      // Pointer to tile image data
    void* image;        // Pointer to the actual texture/image
    
    // Iterate through every tile position in the layer (row-major order)
    for (i = 0; i < map->height; i++) {        // For each row (Y coordinate)
        for (j = 0; j < map->width; j++) {     // For each column (X coordinate)
            // Get the tile ID at this position (remove flip flags to get base GID)
            gid = (layer->content.gids[(i*map->width)+j]) & TMX_FLIP_BITS_REMOVAL;
            
            // Check if this tile ID exists in the map (not empty)
            if (map->tiles[gid] != NULL) {
                // Get tileset and image data for this tile
                ts = map->tiles[gid]->tileset;
                im = map->tiles[gid]->image;
                x = map->tiles[gid]->ul_x;  // Upper-left X in tileset texture
                y = map->tiles[gid]->ul_y;  // Upper-left Y in tileset texture
                w = ts->tile_width;         // Tile width
                h = ts->tile_height;        // Tile height
                
                // Get the actual texture to draw from (either individual tile image or tileset)
                image = im ? im->resource_image : ts->image->resource_image;
                
                // Get the flip flags for this tile (rotation/flipping information)
                flags = (layer->content.gids[(i*map->width)+j]) & ~TMX_FLIP_BITS_REMOVAL;
                
                // Draw the tile at the calculated screen position
                draw_tile(image, x, y, w, h, 
                         j*ts->tile_width,   // Screen X position (tile coordinate * tile width)
                         i*ts->tile_height,  // Screen Y position (tile coordinate * tile height)
                         op, flags);         // Opacity and transformation flags
            }
        }
    }
}

// Draw all layers of the map including game entities
static void draw_all_layers(tmx_map *map, tmx_layer *layers, Animation *anim, Boss *boss) {
    // Load boss death texture for rendering (used in DrawBoss function)
    Texture2D bossDeathTex = LoadTexture("skill7.png");
    
    // Iterate through all layers in the map
    while (layers) {
        // Only draw visible tile layers (skip object layers, hidden layers, etc.)
        if (layers->visible && layers->type == L_LAYER) {
            draw_layer(map, layers); // Draw this tile layer
        }
        layers = layers->next; // Move to next layer
    }
    
    // Draw game entities on top of the tile layers (in correct z-order)
    DrawAnimation(anim);        // Draw player character
    DrawBoss(boss, bossDeathTex); // Draw boss with death texture
    DrawProjectiles();          // Draw all active projectiles
    
    // Clean up the boss death texture
    UnloadTexture(bossDeathTex);
}

// Main map rendering function - draws the entire game scene
void RenderMap(tmx_map *map, Animation *anim, Boss *boss) {
    // Clear the screen with the map's background color
    ClearBackground(int_to_color(map->backgroundcolor));
    
    // Draw all map layers and game entities
    draw_all_layers(map, map->ly_head, anim, boss);

    // HEALTH BAR SYSTEM: Draw minimal health bars above characters
    
    int barWidth = 40;   // Width of health bars in pixels
    int barHeight = 4;   // Height of health bars in pixels

    // PLAYER HEALTH BAR: Draw above player character
    float playerSpriteW = FRAME_WIDTH * anim->scale; // Calculate player sprite width
    float playerBarX = anim->position.x + (playerSpriteW - barWidth) / 2; // Center above player
    float playerBarY = anim->position.y - 8; // Position above player's head

    // Only draw if player position is valid (on screen)
    if (playerBarX > 0 && playerBarY > 0) {
        // Draw background of health bar (gray, represents missing health)
        DrawRectangle((int)playerBarX, (int)playerBarY, barWidth, barHeight, (Color){100, 100, 100, 200});
        
        // Calculate current health ratio (0.0 to 1.0)
        float playerHealthRatio = (float)anim->health / 2000.0f;
        if (playerHealthRatio < 0) playerHealthRatio = 0; // Clamp at 0
        
        // Calculate width of current health portion
        int playerCurrentHealthWidth = (int)(barWidth * playerHealthRatio);
        
        // Draw current health portion with color coding
        if (playerCurrentHealthWidth > 0) {
            Color healthColor;
            // Green when healthy (>60%), Yellow when wounded (30-60%), Red when critical (<30%)
            if (playerHealthRatio > 0.6f) healthColor = GREEN;
            else if (playerHealthRatio > 0.3f) healthColor = YELLOW;
            else healthColor = RED;
            
            DrawRectangle((int)playerBarX, (int)playerBarY, playerCurrentHealthWidth, barHeight, healthColor);
        }
        
        // Draw border around health bar for better visibility
        DrawRectangleLines((int)playerBarX, (int)playerBarY, barWidth, barHeight, (Color){0, 0, 0, 100});
    }

    // BOSS HEALTH BAR: Draw above boss (only if boss is alive and on screen)
    if (!boss->isDead && boss->position.x > 0 && boss->position.y > 0) {
        float bossW = (boss->skillRight.width / 6) * 1.8f; // Calculate boss width
        float bossBarX = boss->position.x + (bossW - barWidth) / 2; // Center above boss
        float bossBarY = boss->position.y - 8; // Position above boss

        // Draw background of boss health bar
        DrawRectangle((int)bossBarX, (int)bossBarY, barWidth, barHeight, (Color){100, 100, 100, 200});
        
        // Calculate boss health ratio
        float bossHealthRatio = (float)boss->health / 10000.0f;
        if (bossHealthRatio < 0) bossHealthRatio = 0; // Clamp at 0
        
        // Draw current boss health (always red for boss)
        int bossCurrentHealthWidth = (int)(barWidth * bossHealthRatio);
        if (bossCurrentHealthWidth > 0) {
            DrawRectangle((int)bossBarX, (int)bossBarY, bossCurrentHealthWidth, barHeight, RED);
        }
        
        // Draw border around boss health bar
        DrawRectangleLines((int)bossBarX, (int)bossBarY, barWidth, barHeight, (Color){0, 0, 0, 100});
    }

    // HUD (Heads-Up Display): Always visible health information in top-left corner
    
    int hudX = 10;        // HUD X position from left edge
    int playerHudY = 10;  // Player HUD Y position
    int bossHudY = 40;    // Boss HUD Y position (below player HUD)
    int fontSize = 20;    // Font size for HUD text
    int padding = 8;      // Padding around HUD text for background
    
    // PLAYER HUD: Always show player health status
    char playerHudText[64];
    sprintf(playerHudText, "Player HP: %d / 2000", anim->health); // Format health text
    Vector2 playerTextSize = MeasureTextEx(GetFontDefault(), playerHudText, fontSize, 1); // Measure text size
    
    // Draw semi-transparent background for player HUD (higher opacity for readability)
    DrawRectangle(hudX - padding, playerHudY - padding, 
                 playerTextSize.x + padding * 2, playerTextSize.y + padding * 2, 
                 (Color){0, 0, 0, 230}); // Dark semi-transparent background
    
    // Draw border around player HUD background
    DrawRectangleLines(hudX - padding, playerHudY - padding, 
                      playerTextSize.x + padding * 2, playerTextSize.y + padding * 2, 
                      WHITE); // White border
    
    // Draw player HUD text with shadow effect for better readability
    DrawText(playerHudText, hudX + 1, playerHudY + 1, fontSize, BLACK); // Black shadow (offset)
    DrawText(playerHudText, hudX, playerHudY, fontSize, GREEN); // Green main text
    
    // BOSS HUD: Always show boss health status (even when dead)
    char bossHudText[64];
    Vector2 bossTextSize;
    
    // Format boss HUD text based on whether boss is alive or dead
    if (!boss->isDead) {
        sprintf(bossHudText, "Boss HP: %d / 10000", boss->health); // Show current health
        bossTextSize = MeasureTextEx(GetFontDefault(), bossHudText, fontSize, 1);
    } else {
        sprintf(bossHudText, "Boss: DEFEATED"); // Show defeated status
        bossTextSize = MeasureTextEx(GetFontDefault(), bossHudText, fontSize, 1);
    }
    
    // Draw semi-transparent background for boss HUD
    DrawRectangle(hudX - padding, bossHudY - padding, 
                 bossTextSize.x + padding * 2, bossTextSize.y + padding * 2, 
                 (Color){0, 0, 0, 230});
    
    // Draw border around boss HUD background
    DrawRectangleLines(hudX - padding, bossHudY - padding, 
                      bossTextSize.x + padding * 2, bossTextSize.y + padding * 2, 
                      WHITE);
    
    // Draw boss HUD text with shadow effect
    if (!boss->isDead) {
        DrawText(bossHudText, hudX + 1, bossHudY + 1, fontSize, BLACK); // Shadow
        DrawText(bossHudText, hudX, bossHudY, fontSize, RED); // Red text for alive boss
    } else {
        DrawText(bossHudText, hudX + 1, bossHudY + 1, fontSize, BLACK); // Shadow
        DrawText(bossHudText, hudX, bossHudY, fontSize, GRAY); // Gray text for defeated boss
    }
}