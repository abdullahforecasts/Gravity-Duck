#include "GameObject.h"


GameObject::GameObject(float x, float y) : x(x), y(y) {}

void GameObject::SetPosition(float newX, float newY) {
    x = newX;
    y = newY;
    hitbox.x = newX;
    hitbox.y = newY;
}

bool GameObject::CheckCollision(const GameObject& other) const {
    return CheckCollisionRecs(hitbox, other.hitbox);
}

void GameObject::MirrorTexture(bool mirror) {
    shouldMirror = mirror;
}

void GameObject::LoadTexture(const std::string& path) {
    if (texture.id != 0) {
        UnloadTexture(texture);
    }
    texture = ::LoadTexture(path.c_str());
    if (texture.id == 0) {
        TraceLog(LOG_ERROR, "Failed to load texture: %s", path.c_str());
    }
}

void GameObject::LoadSound(const string& path) {
    sound = ::LoadSound(path.c_str());
}

void GameObject::DrawTexturePro(float rotation, Color tint) {
    if (texture.id != 0) {
        Rectangle source = { 0, 0,
                          shouldMirror ? -(float)texture.width : (float)texture.width,
                          (float)texture.height };
        Rectangle dest = { x + hitbox.width / 2, y + hitbox.height / 2, hitbox.width, hitbox.height };
        Vector2 origin = { hitbox.width / 2, hitbox.height / 2 };
        ::DrawTexturePro(texture, source, dest, origin, rotation, tint);
    }
}

//GameObject::~GameObject() {
//    if (texture.id != 0) UnloadTexture(texture);
//    if (sound.frameCount != 0) UnloadSound(sound);
//}
