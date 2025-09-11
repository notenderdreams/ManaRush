#include <raylib.h>
#include "music.h"
#include "interface.h"
#include "character.h"
#include <stdlib.h>

#define screenWidth 1450
#define screenHeight 850
const int characterSpeed = 10;
const int gravity = 1;
const int groundYPos = (3 * screenHeight) / 4;
float scale = 2.0f;

static void Cleanup(void) {
    CloseAudioDevice();
    CloseWindow();
}

int main(void) {
    InitWindow(screenWidth, screenHeight, "Mana Rush");
    InitAudioDevice();
    atexit(Cleanup);

    GameState state = MENU;
    Rectangle startButton = {screenWidth/2.0f - 100, screenHeight/2.0f - 60, 200, 50};
    Rectangle exitButton  = {screenWidth/2.0f - 100, screenHeight/2.0f + 20, 200, 50};

    Character player = InitCharacter("cropped_AnimationSheet_Character.png", groundYPos, scale);

    Sound footstepSound = LoadSoundOrExit("running.mp3");
    Sound landingSound  = LoadSoundOrExit("landing.mp3");

    SetTargetFPS(60);

    while (!WindowShouldClose() && state != EXIT) {
        Vector2 mousePoint = GetMousePosition();

        if (state == MENU) {
            state = HandleMenu(mousePoint, startButton, exitButton);
            DrawMenu(mousePoint, startButton, exitButton, screenWidth, screenHeight);
        } else if (state == GAME) {
            UpdateCharacter(&player, groundYPos, gravity, characterSpeed, footstepSound, landingSound);
            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawCharacter(&player);
            EndDrawing();
        }
    }
    return EXIT_SUCCESS;
}
