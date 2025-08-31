#include "music.h"
#include <stdlib.h>
#include <stdio.h>

static void ShowErrorAndExit(const char *errorMessage) {
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText(errorMessage, 20, 20, 20, BLACK);
        EndDrawing();
    }
    exit(EXIT_FAILURE);
}
Sound LoadSoundOrExit(const char *filename) {
    Sound s = LoadSound(filename);
    if (s.frameCount == 0) {
        ShowErrorAndExit(TextFormat("ERROR: Couldn't load %s.", filename));
    }
    return s;
}
