#include "Ghost.h"

Ghost::Ghost(float x, float y, MovementDirection dir, float start, float end)
    : KillerObject(x, y, Constants::GHOST_DIM, Constants::GHOST_DIM, dir, start, end,
        "assets/pngs/ghost.png", "assets/sounds/ghost_death.mp3") {
}

void Ghost::PlayDeathSound() {
    PlaySound(sound);
}


void Ghost::Draw() {
    // Base drawing
    if (texture.id != 0) {
        DrawTexturePro(0, WHITE);
    }

    // Flicker between white and light pink
    float ghostPulse = (sinf(GetTime() * 3.0f) * 0.5f + 0.5f);
    Color ghostColor = Color{
        255,
        static_cast<unsigned char>(165 + (255 - 182) * ghostPulse), // green
        static_cast<unsigned char>(193 + (255 - 193) * ghostPulse), // blue
        150
    };

    // Main glow
    DrawGlowEffect(ghostColor, 1.2f, 3.0f);

    // Wispy trail
    for (int i = 0; i < 3; i++) {
        float offset = ghostPulse * 5 * (i + 1);
        float alpha = 100 - i * 30;
        DrawCircle(
            x + hitbox.width / 2 + (movingForward ? -offset : offset),
            y + hitbox.height / 2,
            hitbox.width / 2 - i * 5,
            Fade(ghostColor, alpha / 255.0f)
        );
    }
}
