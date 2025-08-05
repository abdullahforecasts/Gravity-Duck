#pragma once
#include "GameObject.h"
#include "raylib.h"
#include "Righter.h"
#include "Egg.h"


// Forward declaration
class Game;
class Egg;
class Righter;

class Duck : public GameObject {
private:
    enum class GravityDirection { DOWN, UP, LEFT, RIGHT };
    GravityDirection gravityDir;
    Vector2 velocity;
    bool isAlive = true;
    bool isFacingRight = true;
    bool isOnWall = false;
    bool isFloating = false;
    float flipCooldown = 0.0f;
    bool wantToFlip = false;
    bool hasTouchedRighter = false;
    bool isTouchingRighterThisFrame = false;
   



    bool isTouchingHorizontalWall = false; // Left or Right
    bool isTouchingVerticalWall = false;   // Top or Bottom

    //shop stuff
    static Texture2D duckTextures[4];  // For multiple skins
    int currentSkinIndex = 0;          // Track current skin

public:
    Duck(float x, float y);
    ~Duck() override = default;

    void Update(float deltaTime) override;
    void Draw() override;
    void OnCollision(GameObject* other) override;

    void FlipGravity();
    void MoveLeft();
    void MoveRight();
    void ApplyGravity(float deltaTime);
    void ResetMovement();

    bool IsAlive() const { return isAlive; }
    void Die() {
        isAlive = false;
        ResetMovement();
    }

    Egg::Orientation GetCurrentOrientation() const;

    //shop stuff
    void SetSkin(int skinIndex);
    static void LoadTextures();
    static void UnloadTextures();

private:
    void HandleInput(float deltaTime);
    void HandleWallCollision(GameObject* wall);
    void HandleRighterCollision(Righter* righter);
    void UpdatePosition();
    float Clamp(float value, float min, float max);
};


