#pragma once
#include "GameObject.h"
#include <vector>

class Shooter : public GameObject {
public:
    enum Orientation {
        RIGHT,  // Shoots to the right
        LEFT    // Shoots to the left
    };

private:
    Orientation orientation;
    float shootTimer = 0;
    const float shootInterval = 1.3f;
    const float activationRange = 160.0f;
    bool isActive = false;

    struct Bullet {
        float x, y;
        bool movingRight;
        bool active = true;
    };
    std::vector<Bullet> bullets;

public:
    Shooter(float x, float y, Orientation orient);
    void Update(float deltaTime) override;
    void Draw() override;
    void OnCollision(GameObject* other) override {}

    void Shoot();
    void CheckBulletCollisions();
    const std::vector<Bullet>& GetBullets() const { return bullets; }
    bool IsActive() const { return isActive; }
};