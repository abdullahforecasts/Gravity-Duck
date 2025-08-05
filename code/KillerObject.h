#pragma once
#include "GameObject.h"
#include <string>

class KillerObject : public GameObject {
public:
    enum MovementDirection { VERTICAL, HORIZONTAL };

    KillerObject(float x, float y, float width, float height,
        MovementDirection dir, float start, float end,
        const string& texturePath, const string& soundPath);
    virtual ~KillerObject() = default;

    void Update(float deltaTime) override;
    void Draw() override;
    void OnCollision(GameObject* other) override;
    float Clamp(float value, float min, float max) {
        return value < min ? min : (value > max ? max : value);
    }
protected:
    MovementDirection direction;
    float startPos;
    float endPos;
    float speed;
    bool movingForward = true;
    virtual void PlayDeathSound() = 0;

    void DrawGlowEffect(Color color, float intensity, float pulseSpeed = 5.0f);
};