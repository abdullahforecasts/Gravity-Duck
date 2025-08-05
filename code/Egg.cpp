// Egg.cpp
#include "Egg.h"
#include "Duck.h"
#include "Game.h"
#include "raylib.h"
#include <cmath>
#include "Constants.h"

Texture2D Egg::eggTexture = { 0 };
Sound Egg::winSound = { 0 };

Egg::Egg(float x, float y, Orientation orient) : GameObject(x, y), orientation(orient) {
    hitbox = { x, y, Constants::EGG_DIM, Constants::EGG_DIM };
    if (eggTexture.id == 0) LoadResources();
}

void Egg::Draw() {
    if (!collected && eggTexture.id != 0) {
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

        // Use Raylib's DrawTexturePro directly
        Rectangle source = { 0, 0,
                           (float)eggTexture.width,
                           (float)eggTexture.height };
        Rectangle dest = {
            hitbox.x + hitbox.width / 2,
            hitbox.y + hitbox.height / 2,
            hitbox.width,
            hitbox.height
        };
        Vector2 origin = { hitbox.width / 2, hitbox.height / 2 };
       // ::DrawTexturePro(eggTexture, source, dest, origin, rotation, WHITE);


        float flicker = 0.9f + 0.1f * sinf(GetTime() * 10.0f);
        float scale = 1.0f + 0.02f * sinf(GetTime() * 10.0f);

    

       ::DrawTexturePro(eggTexture, source, dest, origin, rotation,
            Fade(WHITE, flicker));
    }
}

void Egg::OnCollision(GameObject* other) {
    if (!collected) {
        Duck* duck = dynamic_cast<Duck*>(other);
        if (duck && duck->GetCurrentOrientation() == orientation) {
            collected = true;
            PlaySound(winSound);
            Game::GetInstance()->NextLevel();

            // Add egg to shop
            Game::GetInstance()->GetShop()->AddEgg();
            Game::GetInstance()->GetShop()->SaveState();

        }
        
    }
}

void Egg::LoadResources() {
    eggTexture = ::LoadTexture("assets/pngs/egg.png");
    winSound = ::LoadSound("assets/sounds/egg.mp3");
}

void Egg::UnloadResources() {
    if (eggTexture.id != 0) UnloadTexture(eggTexture);
    if (winSound.frameCount != 0) UnloadSound(winSound);
}