#ifndef CHARACTER_H
#define CHARACTER_H

#include <raylib.h>

typedef struct {
    Texture2D texture;
    Rectangle frameRec;
    Vector2 position;
    Vector2 velocity;
    int frameWidth;
    unsigned frameIndex;
    unsigned frameDelay;
    unsigned frameDelayCounter;
    float scale;
} Character;
Character InitCharacter(const char *filename, int groundYPos, float scale);
void UpdateCharacter(Character *ch, int groundYPos, int gravity, int speed, Sound footstep, Sound landing);
void DrawCharacter(Character *ch);

#endif
