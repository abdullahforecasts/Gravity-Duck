#pragma once
#include "GameObject.h"

class Righter : public GameObject {
public:
    Righter(float x, float y);
    void Update(float deltaTime) override {}
    void Draw() override;
    void OnCollision(GameObject* other) override;



};