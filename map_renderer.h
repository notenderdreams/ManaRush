#ifndef MAP_RENDERER_H
#define MAP_RENDERER_H

// Include necessary libraries for map rendering functionality
#include "raylib.h"      // Graphics rendering functions
#include "tmx.h"         // TMX map loading and parsing
#include "character.h"   // Character definitions (for rendering player)
#include "boss.h"        // Boss definitions (for rendering boss)

// TMX LOADER CALLBACK FUNCTIONS:
// These functions integrate the TMX library with Raylib's texture system

/**
 * @brief Custom texture loader for TMX library - loads images using Raylib
 * @param path File path to the image/texture to load
 * @return void* Pointer to the loaded Raylib Texture2D
 * 
 * This function is called by the TMX library when it needs to load a texture.
 * It converts TMX's texture loading to use Raylib's texture system.
 */
void *raylib_tex_loader(const char *path);

/**
 * @brief Custom texture deallocator for TMX library - unloads Raylib textures
 * @param ptr Pointer to the Texture2D to unload
 * 
 * This function is called by the TMX library when it needs to free a texture.
 * It properly unloads Raylib textures from GPU memory.
 */
void raylib_free_tex(void *ptr);

/**
 * @brief Convert integer color value to Raylib Color structure
 * @param color Integer color value (typically in 0xRRGGBBAA format)
 * @return Color Raylib Color structure equivalent
 * 
 * TMX stores colors as integers, but Raylib uses Color structs.
 * This function converts between the two formats.
 */
Color int_to_color(int color);

// MAP RENDERING FUNCTION:

/**
 * @brief Main function to render the entire game map with all layers and entities
 * @param map Pointer to the loaded TMX map data
 * @param anim Pointer to the player character animation data
 * @param boss Pointer to the boss enemy data
 * 
 * This function:
 * - Renders all visible tile layers in correct order
 * - Renders object layers (if any)
 * - Handles parallax scrolling for background layers
 * - Draws the player character and boss on top of the map
 * - Manages rendering order (background -> tiles -> entities -> foreground)
 */
void RenderMap(tmx_map *map, Animation *anim, Boss *boss);

#endif