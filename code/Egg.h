// Egg.h
#pragma once
#include "GameObject.h"
#include "raylib.h"

class Game;

class Egg : public GameObject {
public:
    enum Orientation {
        GROUND,
        LEFT_WALL,
        RIGHT_WALL,
        CEILING
    };

    static Texture2D eggTexture;
    static Sound winSound;

    Egg(float x, float y, Orientation orient = GROUND);
    void Draw() override;
    void Update(float deltaTime) override {};
    void OnCollision(GameObject* other) override;

    Orientation GetOrientation() const { return orientation; }

    static void LoadResources();
    static void UnloadResources();

    bool collected = false;
    Orientation orientation;
};