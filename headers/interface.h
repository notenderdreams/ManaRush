#ifndef INTERFACE_H
#define INTERFACE_H

#include <raylib.h>

typedef enum {
    MENU,
    GAME,
    EXIT
} GameState;

GameState HandleMenu(Vector2 mousePoint, Rectangle startButton, Rectangle exitButton);
void DrawMenu(Vector2 mousePoint, Rectangle startButton, Rectangle exitButton, int screenWidth, int screenHeight);

#endif
