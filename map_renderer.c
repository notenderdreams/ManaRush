#include "map_renderer.h"
#include "projectile.h" 
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//Convert to Texture2D
void *raylib_tex_loader(const char *path) {
    Texture2D *returnValue = malloc(sizeof(Texture2D));
    *returnValue = LoadTexture(path);
    return returnValue; 
}

// Textures Deallocator
void raylib_free_tex(void *ptr) {
    UnloadTexture(*((Texture2D *) ptr));
    free(ptr);
}

// Convert integer  to Raylib Color structure
Color int_to_color(int color) {
    tmx_col_bytes res = tmx_col_to_bytes(color);
    return *((Color*)&res);
}

// Draw a single tile from a tileset texture onto the screen
static void draw_tile(
    void *image,                // Actual Texture
    unsigned int sx,            // src x 
    unsigned int sy,            // src y 
    unsigned int sw,            // width
    unsigned int sh,            // height
    unsigned int dx,            // dest-x 
    unsigned int dy,            // dest-y 
    float opacity,
    unsigned int flags
) {

    Texture2D *texture = (Texture2D*) image; 
    int op = 0xFF * opacity; // Calculate alpha value from opacity ..converts "0-1 to 0-255"
    
    DrawTextureRec(
                    *texture, 
                    (Rectangle){sx, sy, sw, sh}, // Source rectangle 
                    (Vector2){dx, dy},           // Destination position (where to draw on screen)
                    (Color){op, op, op, op}      // Opacity
                );    
}


// Draw a single layer from the map
static void draw_layer(tmx_map *map, tmx_layer *layer) {
    unsigned long i, j;
    unsigned int gid, x, y, w, h, flags;
    float op = layer->opacity; // I dont think we will be using it ,Shomoy paile eite remove korbo 
    
    tmx_tileset *ts;    // ptr tileset data
    tmx_image *im;      // ptr to tile image data
    void* image;        // ptr to the actual texture/image
    

    /*
    Iterate through every tile position in the layer (row-major order)
        i -> row ( y-coordinate)
        j -> col ( x-coordinate)
    */

    for (i = 0; i < map->height; i++) {        
        for (j = 0; j < map->width; j++) {     
            // Get the tile ID at this position 
            gid = (layer->content.gids[(i*map->width)+j]) & TMX_FLIP_BITS_REMOVAL;
            
            // Check if this tile ID exists
            if (map->tiles[gid] != NULL) {
                // Get tileset and image data for that specific tile
                ts = map->tiles[gid]->tileset;
                im = map->tiles[gid]->image;
                x = map->tiles[gid]->ul_x;  // Upper-left X in tileset texture
                y = map->tiles[gid]->ul_y;  // Upper-left Y in tileset texture
                w = ts->tile_width;         // Tile width
                h = ts->tile_height;        // Tile height
                
                // Get the actual texture to draw from ? individual tile image : tileset
                image = im ? im->resource_image : ts->image->resource_image;
                
                // Get the flip flags for this tile (rotation/flipping information)
                flags = (layer->content.gids[(i*map->width)+j]) & ~TMX_FLIP_BITS_REMOVAL;
                
                
                draw_tile(
                        image, x, y, w, h, 
                        j*ts->tile_width,   // x position (tile coordinate * tile width)
                        i*ts->tile_height,  // y position (tile coordinate * tile height)
                        op, flags           // Opacity and transformation flags
                    );         
            }
        }
    }
}

// Draw all layers and  entities
static void draw_all_layers(tmx_map *map, tmx_layer *layers, Animation *anim, Boss *boss) {

    // Iterate through all layers in the map
    while (layers) {

        if (layers->visible && layers->type == L_LAYER) { // check visibility and skip object layers
            draw_layer(map, layers); 
        }
        layers = layers->next; 
    }
    
    /*
    Draw game entities on top of the tile layers (in correct z-order)
        -player
        -boss
        -projectiles
    */
    DrawAnimation(anim);        
    DrawBoss(boss); 
    DrawProjectiles();          
}

// Main map rendering function - draws the entire game scene
void RenderMap(tmx_map *map, Animation *anim, Boss *boss) {
    // Set the bg color of the map .Prolly I didn't added any .so its empty basically
    ClearBackground(int_to_color(map->backgroundcolor));
    
    // Draw all layers and game entities
    draw_all_layers(map, map->ly_head, anim, boss);


    //==================================
        //HEALTH BAR SYSTEM: for player
    //=================================
    
    
    int barWidth = 40;   // Width px
    int barHeight = 4;   // Height px

    // PLAYER HEALTH BAR: 
    float playerSpriteW = FRAME_WIDTH * anim->scale;                        //player sprite width
    float playerBarX = anim->position.x + (playerSpriteW - barWidth) / 2;   // Center above player
    float playerBarY = anim->position.y - 8;                                // Position above player


    // Only draw if player position is valid 
    if (playerBarX > 0 && playerBarY > 0) {
        // Draw background of health bar (gray, represents missing health)
        DrawRectangle((int)playerBarX, (int)playerBarY, barWidth, barHeight, (Color){100, 100, 100, 200});
        
        // Calculate current health ratio (0.0 to 1.0)
        float playerHealthRatio = (float)anim->health / 2000.0f;
        if (playerHealthRatio < 0) playerHealthRatio = 0; // Clamp at 0
        
        // get width of current health portion
        int playerCurrentHealthWidth = (int)(barWidth * playerHealthRatio);
        

    /*
    Draw current health 
        -Green (>60%), 
        -Yellow (30-60%), 
        -Red (<30%)
    */

        if (playerCurrentHealthWidth > 0) {
            Color healthColor;
            
            if (playerHealthRatio > 0.6f) healthColor = GREEN;
            else if (playerHealthRatio > 0.3f) healthColor = YELLOW;
            else healthColor = RED;
            
            DrawRectangle((int)playerBarX, (int)playerBarY, playerCurrentHealthWidth, barHeight, healthColor);
        }
        
        // Border 
        DrawRectangleLines((int)playerBarX, (int)playerBarY, barWidth, barHeight, (Color){0, 0, 0, 100});
    }

    //==================================
        //HEALTH BAR SYSTEM: for BOSS
    //=================================
    /*
        Only draw if the boss is alive and present on the screen 
    */
    
    if (!boss->isDead && boss->position.x > 0 && boss->position.y > 0) {
        float bossW = (boss->skillRight.width / 6) * 1.8f; 
        float bossBarX = boss->position.x + (bossW - barWidth) / 2; 
        float bossBarY = boss->position.y - 8; 

        // Draw background of the health bar
        DrawRectangle((int)bossBarX, (int)bossBarY, barWidth, barHeight, (Color){100, 100, 100, 200});
        
        // Calculate health ratio
        float bossHealthRatio = (float)boss->health / 10000.0f;
        if (bossHealthRatio < 0) bossHealthRatio = 0; // limit at 0
        
        // Draw current healt
        int bossCurrentHealthWidth = (int)(barWidth * bossHealthRatio);
        if (bossCurrentHealthWidth > 0) {
            DrawRectangle((int)bossBarX, (int)bossBarY, bossCurrentHealthWidth, barHeight, RED);
        }
        
        // Border
        DrawRectangleLines((int)bossBarX, (int)bossBarY, barWidth, barHeight, (Color){0, 0, 0, 100});
    }

    //==================================
        //HUD
    //=================================
    
    int hudX = 10;        // X position from left edge
    int playerHudY = 10;  // Player HUD Y position
    int bossHudY = 40;    // Boss HUD Y position (below player HUD)
    int fontSize = 20;    
    int padding = 8;      
    
    // PLAYER HUD: Always show 
    char playerHudText[64];
    sprintf(playerHudText, "Player HP: %d / 2000", anim->health); 
    Vector2 playerTextSize = MeasureTextEx(GetFontDefault(), playerHudText, fontSize, 1); // Measure text size
    
    // Bg for player HUD
    DrawRectangle(
                    hudX - padding, 
                    playerHudY - padding, 
                    playerTextSize.x + padding * 2, 
                    playerTextSize.y + padding * 2, 
                    (Color){0, 0, 0, 230}
                ); 
    
    // Border for player HUD 
    DrawRectangleLines(
                        hudX - padding, 
                        playerHudY - padding, 
                        playerTextSize.x + padding * 2, 
                        playerTextSize.y + padding * 2, 
                        WHITE
                    ); 
    
    // player HUD text
    // shadow->black
    // main-text ->green
    DrawText(playerHudText, hudX + 1, playerHudY + 1, fontSize, BLACK); 
    DrawText(playerHudText, hudX, playerHudY, fontSize, GREEN); 
    
    // BOSS HUD(health): Always show even when dead

    char bossHudText[64];
    Vector2 bossTextSize;


   /*
    !Boss->dead
        ?current healt
        : Defeated 
   */ 

    if (!boss->isDead) {
        sprintf(bossHudText, "Boss HP: %d / 10000", boss->health); 
        bossTextSize = MeasureTextEx(GetFontDefault(), bossHudText, fontSize, 1);
    } else {
        sprintf(bossHudText, "Boss: DEFEATED"); 
        bossTextSize = MeasureTextEx(GetFontDefault(), bossHudText, fontSize, 1);
    }
    
    // Bg for boss
    DrawRectangle(
                    hudX - padding, 
                    bossHudY - padding, 
                    bossTextSize.x + padding * 2,
                    bossTextSize.y + padding * 2, 
                    (Color){0, 0, 0, 230}
                ) ;
    
    // Border for boss
    DrawRectangleLines(
                        hudX - padding, 
                        bossHudY - padding, 
                        bossTextSize.x + padding * 2,
                        bossTextSize.y + padding * 2, 
                        WHITE
                    );
    
    /*
        Boss HUD {
            Alive -> Red
            Dead->Gray
        }
        first lines are strokes or shadow whatever 
    */
    if (!boss->isDead) {
        DrawText(bossHudText, hudX + 1, bossHudY + 1, fontSize, BLACK); 
        DrawText(bossHudText, hudX, bossHudY, fontSize, RED); 
    } else {
        DrawText(bossHudText, hudX + 1, bossHudY + 1, fontSize, BLACK); 
        DrawText(bossHudText, hudX, bossHudY, fontSize, GRAY); 
    }
}