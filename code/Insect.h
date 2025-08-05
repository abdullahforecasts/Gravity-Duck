#pragma once
#include "KillerObject.h"

class Insect : public KillerObject {
public:
    Insect(float x, float y, MovementDirection dir, float start, float end);
    ~Insect() override = default;

protected:
    void PlayDeathSound() override;
    void Draw();
};