#pragma once
#include <raylib.h>
#include <string>
#include "Constants.h"

using namespace std;

class GameObject {
protected:
    float x = 0;
    float y = 0;
    Rectangle hitbox{ 0, 0, 0, 0 };
    Texture2D texture{ 0 };
    Sound sound{ 0 };
    bool shouldMirror = false;

public:
    GameObject(float x, float y);
    virtual ~GameObject()=default;

    virtual void Update(float deltaTime) = 0;
    virtual void Draw() = 0;
    virtual void OnCollision(GameObject* other) = 0;

    Rectangle GetHitbox() const { return hitbox; }
    float GetX() const { return x; }
    float GetY() const { return y; }
    void SetPosition(float newX, float newY);

    bool CheckCollision(const GameObject& other) const;
    void MirrorTexture(bool mirror);

    void LoadTexture(const string& path);
    void LoadSound(const string& path);

    void DrawTexturePro(float rotation = 0.0f, Color tint = WHITE);
};