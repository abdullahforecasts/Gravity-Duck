#include "Wall.h"
#include "raylib.h"

Texture2D Wall::wallTexture = { 0 };
int Wall::instanceCount = 0;

Wall::Wall(float x, float y) : GameObject(x, y) {
    hitbox = { x, y, Constants::WALL_DIM, Constants::WALL_DIM };
    if (instanceCount == 0) {
        LoadTexture();
    }
    instanceCount++;
}

Wall::~Wall() {
    instanceCount--;
    if (instanceCount == 0 && wallTexture.id != 0) {
        UnloadTexture();
    }
}

void Wall::Draw() {
    if (wallTexture.id != 0) {
        DrawTextureV(wallTexture, { hitbox.x, hitbox.y }, WHITE);
    }
    else {
        // Fallback rendering
        DrawRectangleRec(hitbox, Color{ 139, 69, 19, 255 }); // Brown
        DrawRectangleLinesEx(hitbox, 1, Color{ 101, 67, 33, 255 }); // Darker brown border
    }
}

void Wall::LoadTexture() {
    wallTexture = ::LoadTexture("assets/pngs/wall.png");
    if (wallTexture.id == 0) {
        TraceLog(LOG_WARNING, "Failed to load wall texture");
    }
}

void Wall::UnloadTexture() {
    if (wallTexture.id != 0) {
        ::UnloadTexture(wallTexture);
        wallTexture = { 0 };
    }
}