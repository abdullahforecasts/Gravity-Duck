#include "Nail.h"
#include "Duck.h"
#include <cmath>

Nail::Nail(float x, float y, Orientation orient) : GameObject(x, y), orientation(orient) {
    hitbox = { x, y, Constants::NAIL_DIM, Constants::NAIL_DIM };
    LoadTexture("assets/pngs/nail.png");
}

void Nail::Draw() {
    if (texture.id != 0) {
        float rotation = 0.0f;
        bool mirror = false;

        switch (orientation) {
        case GROUND:
            rotation = 0.0f;
            break;
        case LEFT_WALL:
            rotation = 90.0f;
            break;
        case RIGHT_WALL:
            rotation = 270.0f;
            break;
        case CEILING:
            rotation = 180.0f;
            break;
        }

        shouldMirror = mirror;
        DrawTexturePro(rotation, WHITE);
    }
    else {
        // Fallback drawing
        DrawCircle(hitbox.x + hitbox.width / 2,
            hitbox.y + hitbox.height / 2,
            hitbox.width / 2,
            WHITE);
    }
}

void Nail::OnCollision(GameObject* other) {
    if (auto duck = dynamic_cast<Duck*>(other)) {
        duck->Die();
    }
}