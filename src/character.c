#include "character.h"
#include <stdlib.h>

static bool IsCharacterOnGround(Character *ch, int groundYPos) {
    return ch->position.y + (ch->texture.height * ch->scale) >= groundYPos;
}
Character InitCharacter(const char *filename, int groundYPos, float scale) {
    Character ch;
    ch.texture = LoadTexture(filename);
    if (ch.texture.id <= 0) {
        while (!WindowShouldClose()) {
            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("ERROR: Couldn't load character texture", 20, 20, 20, BLACK);
            EndDrawing();
        }
        exit(EXIT_FAILURE);
    }
    unsigned numFrames = 8;
    ch.frameWidth = ch.texture.width / numFrames;
    ch.frameRec = (Rectangle){0.0f, 0.0f, (float)ch.frameWidth, (float)ch.texture.height};
    ch.position = (Vector2){720, groundYPos - (ch.texture.height * scale)};
    ch.velocity = (Vector2){0, 0};
    ch.frameIndex = 0;
    ch.frameDelay = 5;
    ch.frameDelayCounter = 0;
    ch.scale = scale;
    return ch;
}
void UpdateCharacter(Character *ch, int groundYPos, int gravity, int speed, Sound footstep, Sound landing) {
    if (IsCharacterOnGround(ch, groundYPos)) {
        if (IsKeyDown(KEY_UP)) ch->velocity.y = -2 * speed;
        if (IsKeyDown(KEY_RIGHT)) {
            ch->velocity.x = speed;
            if (ch->frameRec.width < 0) ch->frameRec.width = -ch->frameRec.width;
            if (!IsSoundPlaying(footstep)) PlaySound(footstep);
        } else if (IsKeyDown(KEY_LEFT)) {
            ch->velocity.x = -speed;
            if (ch->frameRec.width > 0) ch->frameRec.width = -ch->frameRec.width;
            if (!IsSoundPlaying(footstep)) PlaySound(footstep);
        } else {
            ch->velocity.x = 0;
            if (IsSoundPlaying(footstep)) StopSound(footstep);
        }
    }
    bool wasOnGround = IsCharacterOnGround(ch, groundYPos);
    ch->position.x += ch->velocity.x;
    ch->position.y += ch->velocity.y;

    bool nowOnGround = IsCharacterOnGround(ch, groundYPos);
    if (nowOnGround) {
        ch->velocity.y = 0;
        ch->position.y = groundYPos - (ch->texture.height * ch->scale);
        if (!wasOnGround) PlaySound(landing);
    } else {
        ch->velocity.y += gravity;
    }

    ch->frameDelayCounter++;
    if (ch->frameDelayCounter > ch->frameDelay) {
        ch->frameDelayCounter = 0;
        if (ch->velocity.x != 0 || ch->velocity.y != 0) {
            if (nowOnGround) {
                ch->frameIndex = (ch->frameIndex + 1) % 8;
            } else {
                ch->frameIndex = (ch->velocity.y < 0) ? 3 : 4;
            }
            ch->frameRec.x = (float)ch->frameWidth * ch->frameIndex;
        }
    }
}
void DrawCharacter(Character *ch) {
    Rectangle destRec = { ch->position.x, ch->position.y, ch->frameRec.width * ch->scale, ch->frameRec.height * ch->scale };
    DrawTexturePro(ch->texture, ch->frameRec, destRec, (Vector2){0,0}, 0.0f, WHITE);
}
