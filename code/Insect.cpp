#include "Insect.h"

Insect::Insect(float x, float y, MovementDirection dir, float start, float end)
    : KillerObject(x, y, Constants::INSECT_DIM, Constants::INSECT_DIM, dir, start, end,
        "assets/pngs/insect.png", "assets/sounds/insect_death.mp3") {
}

void Insect::PlayDeathSound() {
    PlaySound(sound);
}
void Insect::Draw() {
    // Base drawing
    if (texture.id != 0) {
        DrawTexturePro(0, WHITE);
    }

    // Sparkle effect
    float sparkleTime = GetTime();
    Color sparkleColor = LIME;

    // Main glow
    DrawGlowEffect(sparkleColor, 0.8f, 7.0f);

    // Sparkle particles
    for (int i = 0; i < 5; i++) {
        float angle = sparkleTime * 2 + i * PI * 0.4f;
        float dist = (sinf(sparkleTime * 5 + i) * 0.5f + 0.8f);
        float size = 2 + (sinf(sparkleTime * 10 + i) * 2);

        DrawCircle(
            x + hitbox.width / 2 + cosf(angle) * hitbox.width / 2 * dist,
            y + hitbox.height / 2 + sinf(angle) * hitbox.height / 2 * dist,
            size,
            Fade(sparkleColor, 0.7f)
        );
    }
    Color SpeedLineColor = RED;
    // Speed lines when moving fast
    if (speed > 30) {
        for (int i = 0; i < 3; i++) {
            DrawLineEx(
                Vector2{ x + hitbox.width / 2, y + hitbox.height / 2 },
                Vector2{
                    x + hitbox.width / 2 + (movingForward ? -15 - i * 5 : 15 + i * 5),
                    y + hitbox.height / 2 + (i - 1) * 5
                },
                1.5f,
                Fade(SpeedLineColor, 0.5f - i * 0.15f)
            );
        }
    }
}