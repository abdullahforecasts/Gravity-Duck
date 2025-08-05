
#include "KillerObject.h"
#include "Duck.h"
#include "Game.h"
using namespace std;

KillerObject::KillerObject(float x, float y, float width, float height,
    MovementDirection dir, float start, float end,
    const std::string& texturePath, const std::string& soundPath)
    : GameObject(x, y), direction(dir), startPos(start), endPos(end), speed(Constants::KILLER_OBJECT_SPEED)
{
    hitbox = { x, y, width, height };
    LoadTexture(texturePath);
    LoadSound(soundPath);

    // Initialize the correct position based on direction
    if (direction == HORIZONTAL) {
        this->x = startPos;
        this->y = y;  // Maintain the vertical position
    }
    else {
        this->y = startPos;
        this->x = x;  // Maintain the horizontal position
    }
}

void KillerObject::Update(float deltaTime)
{
    // Determine which axis we're moving on
    float& movingAxisPos = (direction == HORIZONTAL) ? x : y;
    float& fixedAxisPos = (direction == HORIZONTAL) ? y : x;

    float movement = speed * deltaTime;

    if (movingForward) {
        movingAxisPos += movement;

        // Check if we've reached/passed the end position
        if (movingAxisPos >= endPos) {
            movingAxisPos = endPos;
            movingForward = false;
            shouldMirror = true;  // Mirror when moving left/up
        }
    }
    else {
        movingAxisPos -= movement;

        // Check if we've reached/passed the start position
        if (movingAxisPos <= startPos) {
            movingAxisPos = startPos;
            movingForward = true;
            shouldMirror = false;  // Normal orientation when moving right/down
        }
    }

    // Update hitbox position
    hitbox.x = x;
    hitbox.y = y;

    // Ensure we never go out of bounds
    if (direction == HORIZONTAL) {
        x = Clamp(x, min(startPos, endPos), max(startPos, endPos));
    }
    else {
        y = Clamp(y, min(startPos, endPos), max(startPos, endPos));
    }
}

void KillerObject::Draw() {
    if (texture.id != 0) {
        DrawTexturePro(0, WHITE);
    }
}

void KillerObject::OnCollision(GameObject* other) {
    if (dynamic_cast<Duck*>(other)) {
        PlayDeathSound();
        dynamic_cast<Duck*>(other)->Die();
    }
}

void KillerObject::DrawGlowEffect(Color color, float intensity, float pulseSpeed) {
    float pulse = (sinf(GetTime() * pulseSpeed) * 0.5f + 0.5f);
    float radius = hitbox.width * (1.0f + intensity * pulse * 0.3f);

    // Glow effect
    DrawCircleGradient(
        x + hitbox.width / 2,
        y + hitbox.height / 2,
        radius,
        Fade(color, 0.3f * intensity * pulse),
        Fade(color, 0.0f)
    );

    // Pulsing outline
    DrawCircleLines(
        x + hitbox.width / 2,
        y + hitbox.height / 2,
        hitbox.width / 2 + 5 * pulse,
        Fade(color, 0.7f * intensity * pulse)
    );
}