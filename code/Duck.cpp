#include "Duck.h"
#include "Game.h"
#include "Wall.h"
#include "Nail.h"
#include "Righter.h"
#include "Shooter.h"
#include "Ghost.h"
#include "Insect.h"
#include "Brick.h"

Texture2D Duck::duckTextures[4];

Duck::Duck(float x, float y) : GameObject(x, y),
gravityDir(GravityDirection::DOWN),
velocity({ 0, 0 }) {
    hitbox = { x, y, Constants::DUCK_WIDTH, Constants::DUCK_HEIGHT };
    LoadSound("assets/sounds/quack.mp3");

    // Get current skin from shop
    int skin = static_cast<int>(Game::GetInstance()->GetShop()->GetCurrentSkin()) - 1;
    if (skin < 0 || skin >= 4) skin = 0; // fallback to default
    currentSkinIndex = skin;
    texture = duckTextures[currentSkinIndex];

   
}

void Duck::Update(float deltaTime) {
    if (!isAlive) return;

    if (flipCooldown > 0) {
        flipCooldown -= deltaTime;
    }


    HandleInput(deltaTime);

    if (!isOnWall) {
        ApplyGravity(deltaTime);
    }

    if (wantToFlip && isOnWall && flipCooldown <= 0) {
        FlipGravity();
        wantToFlip = false;
    }

    UpdatePosition();

    isOnWall = false;
    if (!isTouchingRighterThisFrame) {
        hasTouchedRighter = false;
    }
    isTouchingRighterThisFrame = false;
}

void Duck::Draw() {
    if (!isAlive) return;

    bool mirror = !isFacingRight;
    float rotation = 0.0f;

    switch (gravityDir) {
    case GravityDirection::UP:
        rotation = 180.0f;
        mirror = isFacingRight;
        break;
    case GravityDirection::LEFT:
        rotation = 90.0f;
        mirror = !isFacingRight;
        break;
    case GravityDirection::RIGHT:
        rotation = 270.0f;
        // Always show non-mirrored sprite on right wall
        mirror = isFacingRight;
        //mirror = false;
        break;
    }

    if (texture.id == 0) {
        DrawRectangle(x, y, hitbox.width, hitbox.height, RED); // Error visual
        TraceLog(LOG_ERROR, "Attempted to draw with invalid texture!");
        return;
    }


    shouldMirror = mirror;
    DrawTexturePro(rotation, WHITE);
}

void Duck::UpdatePosition() {
    x += velocity.x * GetFrameTime();
    y += velocity.y * GetFrameTime();

    x = Clamp(x, 0, Constants::WINDOW_WIDTH - hitbox.width);
    y = Clamp(y, 0, Constants::WINDOW_HEIGHT - hitbox.height);

    hitbox.x = x;
    hitbox.y = y;
}

void Duck::OnCollision(GameObject* other) {
    if (!isAlive) return;

    if (auto wall = dynamic_cast<Wall*>(other)) {
        HandleWallCollision(wall);
    }
    else if (auto nail = dynamic_cast<Nail*>(other)) {
        Die();
    }
    else if (auto righter = dynamic_cast<Righter*>(other)) {
        isTouchingRighterThisFrame = true;
        if (!hasTouchedRighter) {
            HandleRighterCollision(righter);
        }
    }
    else if (auto shooter = dynamic_cast<Shooter*>(other)) {
        Die();
    }
    else if (auto egg = dynamic_cast<Egg*>(other)) {
        egg->OnCollision(this);
    }
    else if (dynamic_cast<Ghost*>(other) || dynamic_cast<Insect*>(other) || dynamic_cast<Brick*>(other)) {
        Die();
    }
}

void Duck::HandleWallCollision(GameObject* wall) {
    Rectangle duckRect = GetHitbox();
    Rectangle wallRect = wall->GetHitbox();

    float penLeft = wallRect.x + wallRect.width - duckRect.x;
    float penRight = duckRect.x + duckRect.width - wallRect.x;
    float penTop = wallRect.y + wallRect.height - duckRect.y;
    float penBottom = duckRect.y + duckRect.height - wallRect.y;

    float minPen = std::min({ penLeft, penRight, penTop, penBottom });

    isTouchingHorizontalWall = false;
    isTouchingVerticalWall = false;

    if (minPen == penLeft) {
        x = wallRect.x + wallRect.width;
        if (velocity.x < 0) velocity.x = 0;
        isTouchingHorizontalWall = true;
    }
    else if (minPen == penRight) {
        x = wallRect.x - duckRect.width;
        if (velocity.x > 0) velocity.x = 0;
        isTouchingHorizontalWall = true;
    }
    else if (minPen == penTop) {
        y = wallRect.y + wallRect.height;
        if (velocity.y < 0) velocity.y = 0;
        isTouchingVerticalWall = true;
    }
    else if (minPen == penBottom) {
        y = wallRect.y - duckRect.height;
        if (velocity.y > 0) velocity.y = 0;
        isTouchingVerticalWall = true;
    }

    hitbox.x = x;
    hitbox.y = y;

    isOnWall = true;
    isFloating = false;
    flipCooldown = 0.0f;
}

void Duck::HandleRighterCollision(Righter* righter) {
    hasTouchedRighter = true;
   
    isFloating = true;

    // First determine the force direction (90° counter-clockwise from current gravity)
    Vector2 forceDirection = { 0, 0 };
    switch (gravityDir) {
    case GravityDirection::DOWN:  // Ground → Left wall
        forceDirection = { -Constants::RIGHTER_FORCE, 0 };
        break;
    case GravityDirection::LEFT:  // Left wall → Roof
        forceDirection = { 0, -Constants::RIGHTER_FORCE };
        break;
    case GravityDirection::UP:  // Roof → Right wall
        forceDirection = { Constants::RIGHTER_FORCE, 0 };
        break;
    case GravityDirection::RIGHT:  // Right wall → Ground
        forceDirection = { 0, Constants::RIGHTER_FORCE };
        break;
    }

    // Then update gravity and facing direction
    switch (gravityDir) {
    case GravityDirection::DOWN:  // Ground → Left wall
        gravityDir = GravityDirection::LEFT;
        // Keep original facing direction
        break;
    case GravityDirection::LEFT:  // Left wall → Roof
        gravityDir = GravityDirection::UP;
        isFacingRight = !isFacingRight;  // Flip facing
        break;
    case GravityDirection::UP:  // Roof → Right wall
        gravityDir = GravityDirection::RIGHT;
        // Keep current facing direction
        break;
    case GravityDirection::RIGHT:  // Right wall → Ground
        gravityDir = GravityDirection::DOWN;
        isFacingRight = !isFacingRight;  // Flip facing
        break;
    }

    // Apply the pre-calculated force
    velocity = forceDirection;
    PlaySound(sound);
}

void Duck::FlipGravity() {
    if (isOnWall && flipCooldown <= 0) {
        bool preserveFacing = (gravityDir == GravityDirection::LEFT || gravityDir == GravityDirection::RIGHT);

        switch (gravityDir) {
        case GravityDirection::DOWN:
            velocity.y = -Constants::JUMP_FORCE;
            gravityDir = GravityDirection::UP;
            break;
        case GravityDirection::UP:
            velocity.y = Constants::JUMP_FORCE;
            gravityDir = GravityDirection::DOWN;
            break;
        case GravityDirection::LEFT:
            velocity.x = Constants::JUMP_FORCE;
            gravityDir = GravityDirection::RIGHT;
            // Don't change facing direction when flipping from left wall
            break;
        case GravityDirection::RIGHT:
            velocity.x = -Constants::JUMP_FORCE;
            gravityDir = GravityDirection::LEFT;
            // Don't change facing direction when flipping from right wall
            break;
        }

        isFloating = true;
        flipCooldown = 0.1f;
        PlaySound(sound);
    }
    else if (!isOnWall) {
        wantToFlip = true;
    }
}

void Duck::MoveLeft() {
    isFacingRight = false;
    switch (gravityDir) {
    case GravityDirection::DOWN:
    case GravityDirection::UP:
        velocity.x = -Constants::MOVING_SPEED;
        break;
    case GravityDirection::LEFT:
        velocity.y = -Constants::MOVING_SPEED;
        break;
    case GravityDirection::RIGHT:
        velocity.y = Constants::MOVING_SPEED;
        break;
    }
}

void Duck::MoveRight() {
    isFacingRight = true;
    switch (gravityDir) {
    case GravityDirection::DOWN:
    case GravityDirection::UP:
        velocity.x = Constants::MOVING_SPEED;
        break;
    case GravityDirection::LEFT:
        velocity.y = Constants::MOVING_SPEED;
        break;
    case GravityDirection::RIGHT:
        velocity.y = -Constants::MOVING_SPEED;
        break;
    }
}

void Duck::ApplyGravity(float deltaTime) {
    switch (gravityDir) {
    case GravityDirection::DOWN: velocity.y += Constants::GRAVITY * deltaTime; break;
    case GravityDirection::UP: velocity.y -= Constants::GRAVITY * deltaTime; break;
    case GravityDirection::LEFT: velocity.x -= Constants::GRAVITY * deltaTime; break;
    case GravityDirection::RIGHT: velocity.x += Constants::GRAVITY * deltaTime; break;
    }
}

//void Duck::HandleInput(float deltaTime) {
//    // Stop movement if no direction keys held
//    if (!IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT) &&
//        !IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN)) {
//        if (gravityDir == GravityDirection::DOWN || gravityDir == GravityDirection::UP) {
//            velocity.x = 0;
//        }
//        else {
//            velocity.y = 0;
//        }
//    }
//
//    switch (gravityDir) {
//    case GravityDirection::DOWN:
//    case GravityDirection::UP:
//        if (IsKeyDown(KEY_LEFT) && !isTouchingHorizontalWall) MoveLeft();
//        if (IsKeyDown(KEY_RIGHT) && !isTouchingHorizontalWall) MoveRight();
//        break;
//
//    case GravityDirection::LEFT:
//        if (IsKeyDown(KEY_UP) && !isTouchingVerticalWall) {
//            isFacingRight = false;
//            velocity.y = -Constants::MOVING_SPEED;
//        }
//        if (IsKeyDown(KEY_DOWN) && !isTouchingVerticalWall) {
//            isFacingRight = true;
//            velocity.y = Constants::MOVING_SPEED;
//        }
//        break;
//
//    case GravityDirection::RIGHT:
//        if (IsKeyDown(KEY_UP) && !isTouchingVerticalWall) {
//            isFacingRight = false;
//            velocity.y = -Constants::MOVING_SPEED;
//        }
//        if (IsKeyDown(KEY_DOWN) && !isTouchingVerticalWall) {
//            isFacingRight = true;
//            velocity.y = Constants::MOVING_SPEED;
//        }
//        break;
//    }
//
//    if (IsKeyPressed(KEY_X)) {
//        wantToFlip = true;
//    }
//}

//void Duck::HandleInput(float deltaTime) {
//    // Stop movement if no direction keys held
//    if (!IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT) &&
//        !IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN)) {
//        if (gravityDir == GravityDirection::DOWN || gravityDir == GravityDirection::UP) {
//            velocity.x = 0;
//        }
//        else {
//            velocity.y = 0;
//        }
//    }
//
//    switch (gravityDir) {
//    case GravityDirection::DOWN:
//    case GravityDirection::UP:
//        if (IsKeyDown(KEY_LEFT)) {
//            velocity.x = -Constants::MOVING_SPEED;
//            isFacingRight = false;
//        }
//        if (IsKeyDown(KEY_RIGHT)) {
//            velocity.x = Constants::MOVING_SPEED;
//            isFacingRight = true;
//        }
//        break;
//
//    case GravityDirection::LEFT:
//    case GravityDirection::RIGHT:
//        if (IsKeyDown(KEY_UP)) {
//            velocity.y = -Constants::MOVING_SPEED;
//            isFacingRight = false;
//        }
//        if (IsKeyDown(KEY_DOWN)) {
//            velocity.y = Constants::MOVING_SPEED;
//            isFacingRight = true;
//        }
//        break;
//    }
//
//    if (IsKeyPressed(KEY_X)) {
//        wantToFlip = true;
//    }
//}

void Duck::HandleInput(float deltaTime) {
    // CASE 1: During Righter journey
    if (isFloating) {
        if (!IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT) &&
            !IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN)) {
            if (gravityDir == GravityDirection::DOWN || gravityDir == GravityDirection::UP) {
                velocity.x = 0;
            }
            else {
                velocity.y = 0;
            }
        }

        switch (gravityDir) {
        case GravityDirection::DOWN:
        case GravityDirection::UP:
            if (IsKeyDown(KEY_LEFT)) {
                velocity.x = -Constants::MOVING_SPEED;
                isFacingRight = false;
            }
            if (IsKeyDown(KEY_RIGHT)) {
                velocity.x = Constants::MOVING_SPEED;
                isFacingRight = true;
            }
            break;

        case GravityDirection::LEFT:
        case GravityDirection::RIGHT:
            if (IsKeyDown(KEY_UP)) {
                velocity.y = -Constants::MOVING_SPEED;
                isFacingRight = false;
            }
            if (IsKeyDown(KEY_DOWN)) {
                velocity.y = Constants::MOVING_SPEED;
                isFacingRight = true;
            }
            break;
        }
    }

    // CASE 2: Normal input when not in Righter journey (with wall checks)
    else {
        if (!IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT) &&
            !IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN)) {
            if (gravityDir == GravityDirection::DOWN || gravityDir == GravityDirection::UP) {
                velocity.x = 0;
            }
            else {
                velocity.y = 0;
            }
        }

        switch (gravityDir) {
        case GravityDirection::DOWN:
        case GravityDirection::UP:
            if (IsKeyDown(KEY_LEFT) && !isTouchingHorizontalWall) MoveLeft();
            if (IsKeyDown(KEY_RIGHT) && !isTouchingHorizontalWall) MoveRight();
            break;

        case GravityDirection::LEFT:
        case GravityDirection::RIGHT:
            if (IsKeyDown(KEY_UP) && !isTouchingVerticalWall) {
                velocity.y = -Constants::MOVING_SPEED;
                isFacingRight = false;
            }
            if (IsKeyDown(KEY_DOWN) && !isTouchingVerticalWall) {
                velocity.y = Constants::MOVING_SPEED;
                isFacingRight = true;
            }
            break;
        }
    }

    // Flip trigger (applies in both cases)
    if (IsKeyPressed(KEY_X)) {
        wantToFlip = true;
    }
}

void Duck::ResetMovement() {
    velocity = { 0, 0 };
}

float Duck::Clamp(float value, float min, float max) {
    return value < min ? min : (value > max ? max : value);
}

Egg::Orientation Duck::GetCurrentOrientation() const {
    switch (gravityDir) {
    case GravityDirection::DOWN: return Egg::GROUND;
    case GravityDirection::UP: return Egg::CEILING;
    case GravityDirection::LEFT: return Egg::LEFT_WALL;
    case GravityDirection::RIGHT: return Egg::RIGHT_WALL;
    default: return Egg::GROUND;
    }
}

//shop stuff

void Duck::LoadTextures() {
    const char* texturePaths[4] = {
        "assets/pngs/duck1.png",
        "assets/pngs/duck2.png",
        "assets/pngs/duck3.png",
        "assets/pngs/duck4.png"
    };

    for (int i = 0; i < 4; ++i) {
        duckTextures[i] = ::LoadTexture(texturePaths[i]);
        if (duckTextures[i].id == 0) {
            TraceLog(LOG_ERROR, "Failed to load duck texture: %s", texturePaths[i]);
        }
    }
}

void Duck::UnloadTextures() {
    for (int i = 0; i < 4; i++) {
        UnloadTexture(duckTextures[i]);
    }
}

void Duck::SetSkin(int skinIndex) {
    if (skinIndex >= 0 && skinIndex < 4) {
        currentSkinIndex = skinIndex;
        texture = duckTextures[currentSkinIndex];
    }
}
