#pragma once
#include <vector>
#include <fstream>
#include "raylib.h"
#include "Shop.h"

using namespace std;
// Forward declarations
class Level;
//class Shop;

class Game {
private:
    static Game* instance;
    vector<Level*> levels;
    int currentLevel;
    bool isRunning;
    bool isPaused;
    bool showStartScreen;
    bool showWinScreen;
    Music bgMusic;
    Texture2D startScreen;
    Texture2D winScreen;
    Rectangle startButton;
    Rectangle restartButton;
    Rectangle backToMenuButton;
    Rectangle exitButton;
    Texture2D exitButtonTexture;

    Music DuckBye;
    bool hasPlayedDuckByeSound = false;



    // Debug variables
    float fps;
    bool showDebugInfo= true;
    ofstream debugFile;

    Game();
    void LogToFile(const string& message);


    //shop stuff
    bool inShop;  // Add this flag
    Shop *shop;

public:
    static Game* GetInstance();
    ~Game();

    void Initialize();
    void Run();
    void Cleanup();

    void NextLevel();
    void PreviousLevel();
    void RestartLevel();
    void RestartGame();
    void TogglePause() { isPaused = !isPaused; }
    void PlayIntroSlides(int totalSlides);


    Level* GetCurrentLevel() const;

   

    int GetCurrentLevelNumber() const { return currentLevel + 1; }
    bool IsGameRunning() const { return isRunning; }
    bool IsExitRequested() const { return IsKeyPressed(KEY_E); }

    //shop stuff
    void EnterShop();
    void ExitShop();
    bool IsInShop() const { return inShop; }
    Shop* GetShop() { return shop; }
    void SetRunning(bool value);

    

private:
    void DrawStartScreen();
    void DrawWinScreen();
    void HandleStartScreenInput();
    void HandleWinScreenInput();
    void DrawDebugInfo();
    void HandleDebugInput();
   
};