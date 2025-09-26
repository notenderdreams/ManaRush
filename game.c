#include "game.h"
#include "map_renderer.h"
#include "raymath.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Initialize the game state with a map file
void InitGame(GameState *game, const char *mapPath) {
    // Set up TMX library to use Raylib's texture loading system
    tmx_img_load_func = raylib_tex_loader;
    tmx_img_free_func = raylib_free_tex;

    // Load the tilemap from file
    game->map = tmx_load(mapPath);
    if (!game->map) {
        tmx_perror("tmx_load");
        exit(1); // Exit if map fails to load
    }

    // Initialize player animation and projectile system
    InitAnimation(&game->player, game->map->tile_height);
    InitProjectiles();

    // Default spawn positions (used if no spawn points found in map)
    Vector2 player_spawn = {343, 471};
    Vector2 boss_spawn = {4695.33, 475};

    // Search through map layers to find spawn points
    tmx_layer *layer = game->map->ly_head;
    while (layer) {
        if (layer->type == L_OBJGR) { // Object group layer
            // Find player spawn point
            if (strcmp(layer->name, "p_spawn") == 0) {
                tmx_object *obj = layer->content.objgr->head;
                if (obj) {
                    player_spawn.x = obj->x;
                    player_spawn.y = obj->y;
                }
            }
            // Find boss spawn point
            else if (strcmp(layer->name, "boss_spawn") == 0) {
                tmx_object *obj = layer->content.objgr->head;
                if (obj) {
                    boss_spawn.x = obj->x;
                    boss_spawn.y = obj->y;
                }
            }
        }
        layer = layer->next;
    }

    // Set player and boss positions
    game->player.position = player_spawn;
    InitBoss(&game->boss, boss_spawn, game->map);

    // Set up the game camera
    game->camera.offset = (Vector2){DISPLAY_W / 2, DISPLAY_H / 2}; // Center camera
    game->camera.target = game->player.position; // Follow player
    game->camera.rotation = 0.0f;   // No rotation
    // Calculate zoom to fit map height with some padding
    game->camera.zoom = (float)DISPLAY_H / (game->map->height * game->map->tile_height) * 1.5f;

    // Initialize game state variables
    game->shootTimer = 0.0f;        // Shooting cooldown timer
    game->wasColliding = false;     // Previous collision state
    game->gameOver = false;         // Game over flag
    game->playerWon = false;        // Victory flag
}

// Update game logic each frame
void UpdateGame(GameState *game, float delta) {
    // Don't update if game is over
    if (game->gameOver) return;

    // Update game entities
    UpdateCharacter(&game->player, delta, game->map, &game->shootTimer);
    UpdateBoss(&game->boss, delta, game->map);
    UpdateProjectiles(delta, game->map, &game->boss);

    // Boss-player collision detection (only if boss is alive)
    if (!game->boss.isDead) {
        // Calculate boss collision rectangle with scaling
        float bossW = (game->boss.skillRight.width / 6) * 1.8f;
        float bossH = game->boss.skillRight.height * 1.8f;
        Rectangle bossRect = {game->boss.position.x, game->boss.position.y, bossW, bossH};
        
        // Calculate player collision rectangle
        float char_w = FRAME_WIDTH * game->player.scale;
        float char_h = FRAME_HEIGHT * game->player.scale;
        Rectangle playerRect = {game->player.position.x, game->player.position.y, char_w, char_h};
        
        // Check collision and apply damage only on first contact
        bool colliding = CheckCollisionRecs(bossRect, playerRect);
        if (colliding && !game->wasColliding) {
            game->player.health -= 200; // Apply damage
            if (game->player.health < 0) game->player.health = 0; // Clamp health
        }
        game->wasColliding = colliding; // Remember collision state for next frame
    }

    // Check game over conditions
    if (game->player.health <= 0) {
        // Player died
        game->gameOver = true;
        game->player.isPaused = true;
    }
    if (game->boss.health <= 0 && game->boss.deathFinished) {
        // Player won (boss defeated and death animation finished)
        game->gameOver = true;
        game->playerWon = true;
        game->player.isPaused = true;
    }

    // Update camera to follow player with constraints
    game->camera.target.x = game->player.position.x; // Always follow X position
    
    // Only follow Y position when player moves upward, prevent downward following
    game->camera.target.y = fmaxf(game->camera.target.y, game->player.position.y);
    
    // Constrain camera Y position to stay within map boundaries
    float map_h = game->map->height * game->map->tile_height;
    float min_cam_y = game->camera.offset.y / game->camera.zoom;
    float max_cam_y = map_h - (DISPLAY_H - game->camera.offset.y) / game->camera.zoom;
    game->camera.target.y = Clamp(game->camera.target.y, min_cam_y, max_cam_y);
}

// Render the game graphics
void RenderGame(GameState *game) {
    // Start 2D mode with camera transformation
    BeginMode2D(game->camera);
    RenderMap(game->map, &game->player, &game->boss); // Render map and entities
    EndMode2D();

    // Display game over message if game has ended
    if (game->gameOver) {
        const char *text = game->playerWon ? "You Win" : "You Died";
        Color color = game->playerWon ? GREEN : RED;
        int textWidth = MeasureText(text, 60);
        // Center the text on screen
        DrawText(text, DISPLAY_W / 2 - textWidth / 2, DISPLAY_H / 2 - 30, 60, color);
    }
}

// Clean up game resources
void CleanupGame(GameState *game) {
    CleanupAnimation(&game->player); // Clean up player resources
    CleanupBoss(&game->boss);        // Clean up boss resources
    CleanupProjectiles();            // Clean up projectile system
    tmx_map_free(game->map);         // Free the loaded map
}