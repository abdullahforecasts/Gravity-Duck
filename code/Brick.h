#pragma once
#include "KillerObject.h"

class Brick : public KillerObject {
public:
    Brick(float x, float y, MovementDirection dir, float start, float end);
    ~Brick() override = default;

protected:
    void PlayDeathSound() override;
    void Draw()override;
};
