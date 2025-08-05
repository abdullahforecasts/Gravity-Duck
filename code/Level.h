#pragma once
#include <vector>
#include <string>
#include "raylib.h"
#include "GameObject.h"

// Forward declarations
class Wall;
class Duck;


class Level {
private:
    string levelFile;
    vector<GameObject*> objects; // Main container for all objects
    vector<Wall*> wallObjects;   // Only for wall collision optimization
    Duck* player;
    GameObject* egg;
    Texture2D background;
    bool isLoaded;
    bool isCompleted;
    int levelNumber;

    void ClearObjects();

public:
    Level(const string& filePath, int number);
    ~Level();

    bool Load();
    void Unload();
    void Update(float deltaTime);
    void Draw();

    bool IsLoaded() const { return isLoaded; }
    bool IsCompleted() const { return isCompleted; }
    Duck* GetPlayer() const { return player; }
    int GetLevelNumber() const { return levelNumber; }
    const vector<Wall*>& GetWalls() const { return wallObjects; }

private:
    bool LoadFromFile();
    void ProcessWallLine(const string& line);
    void ProcessNailLine(const string& line);
    void ProcessRighterLine(const string& line);
    void ProcessEggLine(const std::string& line);
    void ProcessShooterLine(const string& line);
    void ProcessGhostLine(const string& line);
    void ProcessInsectLine(const string& line);
    void ProcessBrickLine(const string& line);

    void CheckCollisions();
};