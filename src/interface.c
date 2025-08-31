#include "interface.h"

GameState HandleMenu(Vector2 mousePoint, Rectangle startButton, Rectangle exitButton) {
    if (CheckCollisionPointRec(mousePoint, startButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        return GAME;
    }
    if (CheckCollisionPointRec(mousePoint, exitButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        return EXIT;
    }
    return MENU;
}
void DrawMenu(Vector2 mousePoint, Rectangle startButton, Rectangle exitButton, int screenWidth, int screenHeight) {
    BeginDrawing();
    for (int i = 0; i < screenHeight; i++) {
        float t = (float) i / screenHeight;
        Color color;
        if (t < 0.5f) {
            color = (Color){ (unsigned char)(40 + (15 - 40) * (t/0.5f)),
                             (unsigned char)(0 + (15 - 0) * (t/0.5f)),
                             (unsigned char)(60 + (60 - 60) * (t/0.5f)), 255 };
        } else {
            float tt = (t - 0.5f) / 0.5f;
            color = (Color){ (unsigned char)(15 + (5 - 15) * tt),
                             (unsigned char)(15 + (50 - 15) * tt),
                             (unsigned char)(60 + (30 - 60) * tt), 255 };
        }
        DrawLine(0, i, screenWidth, i, color);
    }

    const char *title = "MANA RUSH";
    int titleX = screenWidth/2 - MeasureText(title, 70) / 2;
    int titleY = 120;
    DrawText(title, titleX + 4, titleY + 4, 70, DARKPURPLE);
    DrawText(title, titleX, titleY, 70, GOLD);

    Color startColor = CheckCollisionPointRec(mousePoint, startButton) ? (Color){0, 250, 200, 255} : (Color){0, 180, 100, 255};
    DrawRectangleRounded(startButton, 0.3f, 10, startColor);
    DrawText("START", startButton.x + 55, startButton.y + 10, 30, WHITE);

    Color exitColor = CheckCollisionPointRec(mousePoint, exitButton) ? (Color){220, 50, 50, 255} : (Color){120, 20, 40, 255};
    DrawRectangleRounded(exitButton, 0.3f, 10, exitColor);
    DrawText("EXIT", exitButton.x + 70, exitButton.y + 10, 30, WHITE);

    EndDrawing();
}
