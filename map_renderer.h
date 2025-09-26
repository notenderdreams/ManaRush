#ifndef MAP_RENDERER_H
#define MAP_RENDERER_H

#include "raylib.h"      
#include "tmx.h"         
#include "character.h"   
#include "boss.h"        


/**
 * @brief Texture loader for libtmx
 * @param path  to the texture 
 * @return Pointer to the loaded Raylib Texture2D
 */
void *raylib_tex_loader(const char *path);

/**
 * @brief Texture deallocator for libtmx
 * @param ptr Pointer to the Texture2D 
 */
void raylib_free_tex(void *ptr);

/**
 * @brief Convert libtmx int to raylib Color 
 * @param color Integer color value 
 * @return Color Raylib Color structure equivalent
 */
Color int_to_color(int color);


/**
 * @brief Main function to render the entire game map with all layers and entities
 * @param map Pointer to the loaded TMX map data
 * @param anim Pointer to the player character animation data
 * @param boss Pointer to the boss enemy data
 * 
 * This function:
 * - Renders all visible tile layers in correct order
 * - Renders object layers (if any)
 * - Draws the player character and boss 
 * 
 * background -> tiles -> entities -> foreground)
 */
void RenderMap(tmx_map *map, Animation *anim, Boss *boss);

#endif