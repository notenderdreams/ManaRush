#include "raylib.h"
#include "raymath.h"
#include <stddef.h>
#include <stdlib.h>

const int screenWidth = 1450, screenHeight = 850;
const int characterSpeed = 10;
const int gravity = 1;
const int groundYPos = (3 * screenHeight) / 4;
const int jumpUpFrame = 3;
const int jumpDownFrame = 4;
float scale = 2.0f;

typedef enum {
    MENU,
    GAME,
    EXIT
} GameState;

bool isCharacterOnGround(Texture2D *character, Vector2 *characterPosition) {
    return characterPosition->y + (character->height * scale) >= groundYPos;
}
bool isTextureValid(const Texture2D *texture) {
    return texture->id > 0;
}
void showErrorAndExit(const char *errMsg) {
    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText(errMsg, 20, 20, 20, BLACK);
        EndDrawing();
    }
    exit(EXIT_FAILURE);
}
void showCantLoadFileErrorAndExit(const char *filename) {
    showErrorAndExit(TextFormat("ERROR: Couldn't load %s.", filename));
}
void cleanup() {
    CloseAudioDevice(); 
    CloseWindow();
}
int main(void) {
    InitWindow(screenWidth, screenHeight, "Mana Rush");
    InitAudioDevice();
    atexit(cleanup);

    GameState state = MENU;
    Rectangle startButton = { screenWidth/2.0f - 100, screenHeight/2.0f - 60, 200, 50 };
    Rectangle exitButton  = { screenWidth/2.0f - 100, screenHeight/2.0f + 20, 200, 50 };

    const char *filename = "cropped_AnimationSheet_Character2 copy.png";
    Texture2D character = LoadTexture(filename);
    if (!isTextureValid(&character)) {
        showCantLoadFileErrorAndExit(filename);
    }

    unsigned numFrames = 8;
    int frameWidth = character.width / numFrames;
    Rectangle frameRec = { 0.0f, 0.0f, (float)frameWidth, (float)character.height };
    Vector2 characterPosition = {screenWidth / 2.0f, groundYPos - (character.height * scale)};
    Vector2 characterVelocity = {0.0f, 0.0f};
    filename = "running.mp3";
    Sound footstepSound = LoadSound(filename);
    if (!footstepSound.frameCount) {
        showCantLoadFileErrorAndExit(filename);
    }
    filename = "landing.mp3";
    Sound landingSound = LoadSound(filename);
    if (!landingSound.frameCount) {
        showCantLoadFileErrorAndExit(filename);
    }
    unsigned frameDelay = 5;
    unsigned frameDelayCounter = 0;
    unsigned frameIndex = 0;

    SetTargetFPS(60);

    while (!WindowShouldClose() && state != EXIT) {
        Vector2 mousePoint = GetMousePosition();

        if (state == MENU) {
            if (CheckCollisionPointRec(mousePoint, startButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                state = GAME;
            }
            if (CheckCollisionPointRec(mousePoint, exitButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                state = EXIT;
            }

            BeginDrawing();
                for (int i = 0; i < screenHeight; i++) {
                    float t = (float) i / screenHeight;
                    Color color;
                    if (t < 0.5f) {
                        color = (Color){
                            (unsigned char)(40 + (15 - 40) * (t/0.5f)),
                            (unsigned char)(0 + (15 - 0) * (t/0.5f)),
                            (unsigned char)(60 + (60 - 60) * (t/0.5f)),
                            255
                        };
                    } 
                    else {
                        float tt = (t - 0.5f) / 0.5f;
                        color = (Color){
                            (unsigned char)(15 + (5 - 15) * tt),
                            (unsigned char)(15 + (50 - 15) * tt),
                            (unsigned char)(60 + (30 - 60) * tt),
                            255
                        };
                    }
                    DrawLine(0, i, screenWidth, i, color);
                }

                const char *title = "MANA RUSH";
                int titleX = screenWidth/2 - MeasureText(title, 70)/2;
                int titleY = 120;
                DrawText(title, titleX+4, titleY+4, 70, DARKPURPLE); 
                DrawText(title, titleX, titleY, 70, GOLD);           

                Color startColor = CheckCollisionPointRec(mousePoint, startButton) ? (Color){0, 250, 200, 255} : (Color){0, 180, 100, 255};
                DrawRectangleRounded(startButton, 0.3f, 10, startColor);
                DrawText("START", startButton.x + 55, startButton.y + 10, 30, WHITE);

                Color exitColor = CheckCollisionPointRec(mousePoint, exitButton) ? (Color){220, 50, 50, 255} : (Color){120, 20, 40, 255};
                DrawRectangleRounded(exitButton, 0.3f, 10, exitColor);
                DrawText("EXIT", exitButton.x + 70, exitButton.y + 10, 30, WHITE);

            EndDrawing();
        }
        else if (state == GAME) {
            if (isCharacterOnGround(&character, &characterPosition)) {
                if (IsKeyDown(KEY_UP)) {
                    characterVelocity.y = - 2 * characterSpeed;
                }
                if (IsKeyDown(KEY_RIGHT)) {
                    characterVelocity.x = characterSpeed;
                    if (frameRec.width < 0) frameRec.width = -frameRec.width;

                    if (!IsSoundPlaying(footstepSound)) PlaySound(footstepSound);
                } 
                else if (IsKeyDown(KEY_LEFT)) {
                    characterVelocity.x = -characterSpeed;
                    if (frameRec.width > 0) frameRec.width = -frameRec.width;

                    if (!IsSoundPlaying(footstepSound)) PlaySound(footstepSound);
                } 
                else {
                    characterVelocity.x = 0;
                    if (IsSoundPlaying(footstepSound)) StopSound(footstepSound);
                }
            }
            
            bool characterMoving = characterVelocity.x != 0.0f || characterVelocity.y != 0.0f;   
            
            bool wascharacterOnGround = isCharacterOnGround(&character, &characterPosition);
            characterPosition = Vector2Add(characterPosition, characterVelocity);
            bool characterIsOnGround = isCharacterOnGround(&character, &characterPosition);
            if (characterIsOnGround) {
                characterVelocity.y = 0;
                characterPosition.y = groundYPos - (character.height * scale);
                if (!wascharacterOnGround) PlaySound(landingSound);
            } 
            else {
                characterVelocity.y += gravity;
            }
            ++frameDelayCounter;
            if (frameDelayCounter > frameDelay) {
                frameDelayCounter = 0;
                if (characterMoving) {
                    if (characterIsOnGround) {
                        frameIndex = (frameIndex + 1) % numFrames;
                    } else {
                        frameIndex = (characterVelocity.y < 0) ? jumpUpFrame : jumpDownFrame;
                    }
                    frameRec.x = (float) frameWidth * frameIndex;
                }
            }
            BeginDrawing();
            ClearBackground(RAYWHITE);
            Rectangle destRec = {
                characterPosition.x, 
                characterPosition.y, 
                frameRec.width * scale, 
                frameRec.height * scale
            };
            DrawTexturePro(character, frameRec, destRec, (Vector2){0, 0}, 0.0f, WHITE);
            EndDrawing();
        }
    }
    cleanup();
    return EXIT_SUCCESS;
}
