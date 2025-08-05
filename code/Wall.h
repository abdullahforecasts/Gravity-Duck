#pragma once
#include "GameObject.h"

class Wall : public GameObject {
private:
    static Texture2D wallTexture;
    static int instanceCount; // Track active wall instances

public:
    Wall(float x, float y);
    ~Wall() override;

    void Update(float deltaTime) override {}
    void Draw() override;
    void OnCollision(GameObject* other) override {}

    static void LoadTexture();
    static void UnloadTexture();
};