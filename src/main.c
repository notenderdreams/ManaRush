#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>

bool isTextureValid(const Texture2D *texture) {
    return texture->id > 0;
}
int main(void) {
    const int screenWidth = 800, screenHeight = 450, characterSpeed = 5;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "RayLib - 2D Character Animation");
    RenderTexture2D background = LoadRenderTexture(screenWidth, screenHeight);
    const char *filename = "cropped_AnimationSheet_Character.png"; 
    Texture2D character = LoadTexture(filename);
    if (!isTextureValid(&character)) {
        while (!WindowShouldClose()) {
            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText(TextFormat("ERROR: Couldn't load %s.", filename), 20, 20, 20, BLACK);
            EndDrawing();
        }
        return 10;
    }
    unsigned numFrames = 8;
    int frameWidth = character.width / numFrames;
    Rectangle frameRec = { 0.0f, 0.0f, (float)frameWidth, (float)character.height };
    Vector2 characterPosition = { screenWidth / 2.0f, screenHeight / 2.0f };
    Vector2 characterVelocity = { 0.0f, 0.0f };
    float scale = 3.0f;
    unsigned frameDelay = 5, frameDelayCounter = 0, frameIndex = 0;
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_RIGHT) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
            characterVelocity.x = characterSpeed;
            if (frameRec.width < 0) frameRec.width = -frameRec.width;
        } 
        else if (IsKeyDown(KEY_LEFT) || IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
            characterVelocity.x = -characterSpeed;
            if (frameRec.width > 0) frameRec.width = -frameRec.width;
        } 
        else {
            characterVelocity.x = 0;
        }
        bool characterMoving = characterVelocity.x != 0.0f || characterVelocity.y != 0.0f;
        characterPosition = Vector2Add(characterPosition, characterVelocity);
        if (characterPosition.x < 0) characterPosition.x = 0;
        if (characterPosition.x > screenWidth) characterPosition.x = screenWidth;
        ++frameDelayCounter;
        if (frameDelayCounter > frameDelay) {
            frameDelayCounter = 0;
            if (characterMoving) {
                ++frameIndex;
                frameIndex %= numFrames;
                frameRec.x = (float)frameWidth * frameIndex;
            } else {
                frameIndex = 0;
                frameRec.x = 0;
            }
        }
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexturePro(
            character,
            frameRec,
            (Rectangle) {
                characterPosition.x - (frameWidth * scale) / 2,
                characterPosition.y - (character.height * scale) / 2,
                frameWidth * scale,
                character.height * scale
            },
            (Vector2){0, 0},
            0.0f,
            WHITE
        );
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
