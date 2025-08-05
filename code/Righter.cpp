#include "Righter.h"
#include "Duck.h"


Righter::Righter(float x, float y) : GameObject(x, y) {
    // Smaller hitbox
    hitbox = { x, y, 1.0f, 1.0f };

    LoadTexture("assets/pngs/righter.png");
    LoadSound("assets/sounds/shine.mp3");
}

void Righter::Draw() {
    if (texture.id != 0) {
        float rotation = GetTime() * 100.0f;

        // Actual center of the smaller hitbox
        Vector2 center = {
            hitbox.x + hitbox.width / 2,
            hitbox.y + hitbox.height / 2
        };

        Rectangle source = { 0, 0, (float)texture.width, (float)texture.height };
        Rectangle dest = {
            center.x ,  // Shift by half texture width
            center.y ,  // Shift by half texture height
            50, 50          // Texture drawn at full size
        };

        Vector2 origin = { 25, 25 };  // Rotate around center

        ::DrawTexturePro(texture, source, dest, origin, rotation, WHITE);

        // Debug hitbox 
        // DrawRectangleLinesEx(hitbox, 1.0f, RED);

       
    }
    else {
        // Fallback visual
        DrawCircle(
            hitbox.x + hitbox.width / 2,
            hitbox.y + hitbox.height / 2,
            hitbox.width / 2,
            RED
        );
    }
}



void Righter::OnCollision(GameObject* other) {
    if (auto duck = dynamic_cast<Duck*>(other)) {
        PlaySound(sound);
    }
}