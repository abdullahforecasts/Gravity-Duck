#include "Shooter.h"
#include "Duck.h"
#include "Game.h"
#include "raylib.h"
#include "Wall.h"
#include "Level.h"

Shooter::Shooter(float x, float y, Orientation orient) :
    GameObject(x, y), orientation(orient) {
    hitbox = { x, y, Constants::SHOOTER_DIM, Constants::SHOOTER_DIM };
    LoadTexture("assets/pngs/shooter.png");
    shouldMirror = (orient == LEFT);
}

void Shooter::Update(float deltaTime) {
    isActive = false;

    if (auto duck = Game::GetInstance()->GetCurrentLevel()->GetPlayer()) {
        float duckCenterY = duck->GetY() + duck->GetHitbox().height / 2;
        float shooterCenterY = y + hitbox.height / 2;

        if (abs(duckCenterY - shooterCenterY) <= activationRange) {
            float duckX = duck->GetX();
            if ((orientation == RIGHT && duckX > x) ||
                (orientation == LEFT && duckX < x)) {
                isActive = true;
            }
        }
    }

    if (isActive) {
        shootTimer += deltaTime;
        if (shootTimer >= shootInterval) {
            Shoot();
            shootTimer = 0;
        }
    }

    for (auto& bullet : bullets) {
        if (!bullet.active) continue;
        bullet.x += (bullet.movingRight ? 1 : -1) * Constants::BULLET_SPEED * deltaTime;

        if (bullet.x < -100 || bullet.x > Constants::WINDOW_WIDTH + 100) {
            bullet.active = false;
        }
    }

    CheckBulletCollisions();

    // Remove inactive bullets
    bullets.erase(
     remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return !b.active; }),
        bullets.end()
    );
}


void Shooter::Draw() {
    if (texture.id != 0) {
        DrawTexturePro(0, WHITE);
    }

    // Draw bullets with layered glow: outer glow -> middle lime -> inner white
    for (const auto& bullet : bullets) {
        if (!bullet.active) continue;

        float time = GetTime();

        // Outer glow (soft pulsating neon effect)
        float glowRadius = 15 + sinf(time * 5) * 2;
        DrawCircle(bullet.x, bullet.y, glowRadius, Fade(LIME, 0.25f));

        // Middle solid green
        DrawCircle(bullet.x, bullet.y, 6, LIME);

        // Inner bright white core
        DrawCircle(bullet.x, bullet.y, 3.5f, WHITE);
    }

    if (Constants::DEBUG_MODE) {
        DrawRectangleLinesEx(
            { x - 20, y - activationRange, hitbox.width + 40, activationRange * 2 },
            2,
            Fade(GREEN, 0.3f)
        );
    }
}

void Shooter::Shoot() {
    bullets.push_back({
        orientation == RIGHT ? x + hitbox.width : x - 10,
        y + hitbox.height / 2 - 5,
        orientation == RIGHT,
        true
        });
}

void Shooter::CheckBulletCollisions() {
    auto* level = Game::GetInstance()->GetCurrentLevel();
    if (!level) return;

    for (auto& bullet : bullets) {
        if (!bullet.active) continue;

        Rectangle bulletRect = { bullet.x - 10, bullet.y - 10, 20, 20 };

        for (auto wall : level->GetWalls()) {
            if (CheckCollisionRecs(bulletRect, wall->GetHitbox())) {
                bullet.active = false;
                break;
            }
        }
    }
}