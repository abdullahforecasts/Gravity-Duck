#include "Brick.h"

Brick::Brick(float x, float y, MovementDirection dir, float start, float end)
    : KillerObject(x, y, Constants::BRICK_DIM, Constants::BRICK_DIM, dir, start, end,
        "assets/pngs/brick.png", "assets/sounds/brick_death.mp3") {
}

void Brick::PlayDeathSound() {
    PlaySound(sound);
}

void Brick::Draw() {
    if (texture.id != 0) {
        // Base texture
        DrawTexturePro(0, WHITE);
    }

    // Define border rectangle slightly bigger than PNG
    float borderThickness = 1.0f;
    Rectangle border = {
        hitbox.x - borderThickness,
        hitbox.y - borderThickness,
        hitbox.width + 2 * borderThickness,
        hitbox.height + 2 * borderThickness
    };

    // Red border outside PNG
    DrawRectangleLinesEx(border, borderThickness, ORANGE);

    // Crackling sparks outside the red border
    for (int i = 0; i < 8; i++) {
        float t = GetTime() + i;
        float phase = fmodf(t * 2.0f, 1.0f);
        if (phase < 0.3f) {
            float randX = border.x + (rand() % (int)border.width);
            float randY = border.y + (rand() % 2 == 0 ? 0 : border.height);
            DrawCircle(randX, randY, 2, RED);
        }

        if (phase < 0.3f) {
            float randY = border.y + (rand() % (int)border.height);
            float randX = border.x + (rand() % 2 == 0 ? 0 : border.width);
            DrawCircle(randX, randY, 2, RED);
        }
    }
}






