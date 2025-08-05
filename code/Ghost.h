#pragma once
#include "KillerObject.h"

class Ghost : public KillerObject {
public:
    Ghost(float x, float y, MovementDirection dir, float start, float end);
    ~Ghost() override = default;

protected:
    void PlayDeathSound() override;
    void Draw();
};