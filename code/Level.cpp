#include "Level.h"
#include "Wall.h"
#include "Nail.h"
#include "Righter.h"
#include "Shooter.h"
#include "Egg.h"
#include "Brick.h"
#include "Insect.h"
#include "Ghost.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "Duck.h"
using namespace std;

Level::Level(const string& filePath, int number) :
    levelFile(filePath), isLoaded(false), isCompleted(false),
    player(nullptr), egg(nullptr), levelNumber(number) {
    background = { 0 };
}

Level::~Level() {
    Unload();
}

bool Level::Load() {
    Unload();
    if (!LoadFromFile()) return false;

    background = LoadTexture("assets/pngs/background.png");
    isLoaded = true;
    return true;
}

void Level::Unload() {
    if (isLoaded) {
        ClearObjects();
        if (background.id != 0) UnloadTexture(background);
        background = { 0 };
        isLoaded = false;
        isCompleted = false;
    }
}

void Level::Update(float deltaTime) {
    if (!isLoaded || !player) return;

    // Safe iteration by working with indices
    for (size_t i = 0; i < objects.size(); ) {
        if (objects[i]) {
            objects[i]->Update(deltaTime);
            i++;
        }
        else {
            objects.erase(objects.begin() + i);
        }
    }

    // Update walls (no physics, just for drawing)
    for (auto wall : wallObjects) {
        if (wall) wall->Update(deltaTime);
    }

    if (player) player->Update(deltaTime);
    CheckCollisions();

    // Check if egg is collected
    if (egg && dynamic_cast<Egg*>(egg)->collected) {
        isCompleted = true;
    }
}

void Level::Draw() {
    if (!isLoaded) return;

    DrawTexture(background, 0, 0, WHITE);
    for (auto obj : objects) {
        if (obj) obj->Draw();
    }
    if (player) player->Draw();
}


bool Level::LoadFromFile() {
    ifstream file(levelFile);
    if (!file.is_open()) {
        TraceLog(LOG_ERROR, "Failed to open level file: %s", levelFile.c_str());
        return false;
    }

     string line;
    char section = '\0';

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '/') continue;

        // Check if line starts a new section
        if (isalpha(line[0]) && line.size() == 1) {
            section = line[0];
            continue;
        }

        switch (section) {
        case 's': { // Start point (Duck)
            float x, y;
            std::istringstream ss(line);
            ss >> x >> y;
           
            player = new Duck(x, y);
            
            break;
        }
        case 'e': { // Egg (win point)
           
            ProcessEggLine(line);
            break;
        }
        case 'n': // Nails
            ProcessNailLine(line);
            break;
        case 'r': // Righters
            ProcessRighterLine(line);
            break;
        case 'f': // Shooters
            ProcessShooterLine(line);
            break;
        case 'G': // Ghost
            ProcessGhostLine(line);
            break;
        case 'I': // Insect
            ProcessInsectLine(line);
            break;
        case 'B': // Brick
            ProcessBrickLine(line);
            break;
        default: // Walls (default section)
            ProcessWallLine(line);
            break;
        }
    }

    file.close();
    return true;
}

void Level::ProcessWallLine(const string& line) {
    float x1, y1, x2, y2;
    istringstream ss(line);
    ss >> x1 >> y1 >> x2 >> y2;

    if (x1 == x2) { // Vertical wall
        int count = static_cast<int>((y2 - y1) / Constants::WALL_DIM);
        for (int i = 0; i <= count; i++) {
            Wall* wall = new Wall(x1, y1 + i * Constants::WALL_DIM);
            objects.push_back(wall);
            wallObjects.push_back(wall);
        }
    }
    else { // Horizontal wall
        int count = static_cast<int>((x2 - x1) / Constants::WALL_DIM);
        for (int i = 0; i <= count; i++) {
            Wall* wall = new Wall(x1 + i * Constants::WALL_DIM, y1);
            objects.push_back(wall);
            wallObjects.push_back(wall);
        }
    }
}

void Level::ProcessNailLine(const string& line) {
    std::istringstream ss(line);
    std::string firstToken;
    ss >> firstToken;

    // Check if this is a misclassified wall line
    if (firstToken.find('.') != std::string::npos ||
        (firstToken.size() > 1 && isdigit(firstToken[0]))) {
        ProcessWallLine(line);
        return;
    }

    Nail::Orientation orientation = Nail::GROUND;
    float x1, y1, x2, y2;

    if (firstToken.length() == 1 && isalpha(firstToken[0])) {
        char orientChar = tolower(firstToken[0]);
        switch (orientChar) {
        case 'l': orientation = Nail::LEFT_WALL; break;
        case 'r': orientation = Nail::RIGHT_WALL; break;
        case 'c': orientation = Nail::CEILING; break;
        case 'g': orientation = Nail::GROUND; break;
        default: orientation = Nail::GROUND; break;
        }

        if (!(ss >> x1 >> y1 >> x2 >> y2)) {
            TraceLog(LOG_WARNING, "Invalid nail line: %s", line.c_str());
            return;
        }
    }
    else {
        ss.clear();
        ss.seekg(0);
        if (!(ss >> x1 >> y1 >> x2 >> y2)) {
            TraceLog(LOG_WARNING, "Invalid nail line: %s", line.c_str());
            return;
        }
    }

    // Compute tile-wise nail placement
    float dx = x2 - x1;
    float dy = y2 - y1;
    float distance = sqrt(dx * dx + dy * dy);
    int nailCount = static_cast<int>(distance / Constants::NAIL_DIM);

    if (distance > 0) {
        dx /= distance;
        dy /= distance;
    }

    for (int i = 0; i <= nailCount; i++) {
        float nailX = x1 + dx * i * Constants::NAIL_DIM;
        float nailY = y1 + dy * i * Constants::NAIL_DIM;
        objects.push_back(new Nail(nailX, nailY, orientation));
    }
}

void Level::ProcessRighterLine(const string& line) {
    float x, y;
    std::istringstream ss(line);
    ss >> x >> y;
    objects.push_back(new Righter(x, y));
}

void Level::ProcessEggLine(const string& line) {
    std::istringstream ss(line);
    std::string firstToken;
    ss >> firstToken;

    Egg::Orientation orientation = Egg::GROUND;
    float x, y;

    if (firstToken.length() == 1 && isalpha(firstToken[0])) {
        char orientChar = tolower(firstToken[0]);
        switch (orientChar) {
        case 'l': orientation = Egg::LEFT_WALL; break;
        case 'r': orientation = Egg::RIGHT_WALL; break;
        case 'c': orientation = Egg::CEILING; break;
        case 'g': orientation = Egg::GROUND; break;
        default: orientation = Egg::GROUND; break;
        }

        if (!(ss >> x >> y)) {
            TraceLog(LOG_WARNING, "Invalid egg line: %s", line.c_str());
            return;
        }
    }
    else {
        ss.clear();
        ss.seekg(0);
        if (!(ss >> x >> y)) {
            TraceLog(LOG_WARNING, "Invalid egg line: %s", line.c_str());
            return;
        }
    }

    egg = new Egg(x, y, orientation);
    objects.push_back(egg);

    this->egg = egg;  // Store reference to the level's egg
}

void Level::ProcessShooterLine(const string& line) {
    std::istringstream ss(line);
    std::string firstToken;
    ss >> firstToken;

    Shooter::Orientation orientation = Shooter::RIGHT;
    float x, y;

    if (firstToken.length() == 1 && (firstToken[0] == 'r' || firstToken[0] == 'l')) {
        orientation = (firstToken[0] == 'r') ? Shooter::RIGHT : Shooter::LEFT;
        if (!(ss >> x >> y)) {
            TraceLog(LOG_WARNING, "Invalid shooter line: %s", line.c_str());
            return;
        }
    }
    else {
        ss.clear();
        ss.seekg(0);
        if (!(ss >> x >> y)) {
            TraceLog(LOG_WARNING, "Invalid shooter line: %s", line.c_str());
            return;
        }
    }

    objects.push_back(new Shooter(x, y, orientation));
}

void Level::ProcessGhostLine(const string& line) {
    std::istringstream ss(line);
    char dir;
    float x1, y1, x2, y2;
    ss >> dir >> x1 >> y1 >> x2 >> y2;

    KillerObject::MovementDirection direction = (tolower(dir) == 'v') ?
        KillerObject::VERTICAL : KillerObject::HORIZONTAL;

    if (direction == KillerObject::HORIZONTAL) {
        objects.push_back(new Ghost(x1, y1, direction, x1, x2));
    }
    else {
        objects.push_back(new Ghost(x1, y1, direction, y1, y2));
    }
}

void Level::ProcessInsectLine(const string& line) {
    std::istringstream ss(line);
    char dir;
    float x1, y1, x2, y2;
    ss >> dir >> x1 >> y1 >> x2 >> y2;

    KillerObject::MovementDirection direction = (tolower(dir) == 'v') ?
        KillerObject::VERTICAL : KillerObject::HORIZONTAL;

    if (direction == KillerObject::HORIZONTAL) {
        objects.push_back(new Insect(x1, y1, direction, x1, x2));
    }
    else {
        objects.push_back(new Insect(x1, y1, direction, y1, y2));
    }
}

void Level::ProcessBrickLine(const string& line) {
    std::istringstream ss(line);
    char dir;
    float x1, y1, x2, y2;
    ss >> dir >> x1 >> y1 >> x2 >> y2;

    KillerObject::MovementDirection direction = (tolower(dir) == 'v') ?
        KillerObject::VERTICAL : KillerObject::HORIZONTAL;

    if (direction == KillerObject::HORIZONTAL) {
        objects.push_back(new Brick(x1, y1, direction, x1, x2));
    }
    else {
        objects.push_back(new Brick(x1, y1, direction, y1, y2));
    }
}

void Level::CheckCollisions() {
    if (!player) return;

    // Check egg collision
    if (egg && CheckCollisionRecs(player->GetHitbox(), egg->GetHitbox())) {
        Duck* duck = dynamic_cast<Duck*>(player);
        Egg* collidedEgg = dynamic_cast<Egg*>(egg);
        if (duck && collidedEgg && duck->GetCurrentOrientation() == collidedEgg->GetOrientation()) {
            isCompleted = true;
            collidedEgg->OnCollision(player);
        }
        return;
    }

    // Check other collisions
    for (auto obj : objects) {
        if (CheckCollisionRecs(player->GetHitbox(), obj->GetHitbox())) {
            player->OnCollision(obj);
            obj->OnCollision(player);
        }

        // Special check for shooter bullets
        if (auto shooter = dynamic_cast<Shooter*>(obj)) {
            for (const auto& bullet : shooter->GetBullets()) {
                if (bullet.active && CheckCollisionRecs(
                    player->GetHitbox(),
                    { bullet.x - 10, bullet.y - 10, 20, 20 })) {
                    dynamic_cast<Duck*>(player)->Die();
                    break;
                }
            }
        }
    }
}

void Level::ClearObjects() {
    // Clear all game objects
    for (auto obj : objects) {
        if (obj != player && obj != egg) {
            delete obj;
        }
    }
    objects.clear();
    wallObjects.clear(); // Just clear pointers, objects already deleted

    // Delete special objects
    if (player) {
        delete player;
        player = nullptr;
    }
    if (egg) {
        delete egg;
        egg = nullptr;
    }
}