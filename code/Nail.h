#pragma once
#include "GameObject.h"

class Nail : public GameObject {
public:
    enum Orientation { GROUND, LEFT_WALL, RIGHT_WALL, CEILING };

    Nail(float x, float y, Orientation orient = GROUND);
    void Update(float deltaTime) override {};
    void Draw() override;
    void OnCollision(GameObject* other) override;

private:
    Orientation orientation;
};