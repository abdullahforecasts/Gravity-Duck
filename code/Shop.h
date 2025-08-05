#pragma once
#include "raylib.h"
#include <vector>
#include <string>
#include <fstream>


// Forward declaration
class Game;


class Shop {
public:
    enum ShopState { MENU, SKINS, EXCHANGE, GAMBLE };
    enum DuckSkin { DEFAULT = 1, BUYABLE1 = 2, BUYABLE2 = 3, GAMBLE_SKIN = 4 };

    Shop();
    ~Shop();

    void Update();
    void Draw();
    void SaveState();
    void LoadState();

    // Navigation
    void GoToMenu();
    void GoToSkins();
    void GoToExchange();
    void GoToGamble();

    // Getters
    DuckSkin GetCurrentSkin() const { return currentSkin; }
    Texture2D GetDuckTexture(int index) const { return duckSkins[index]; }


    void AddEgg() { eggs++; }
    void AddCoins(float amount) { coins += amount; }
    void SetCurrentSkin(DuckSkin skin) { currentSkin = skin; }
    float GetCoins() const { return coins; }
    int GetEggs() const { return eggs; }


    void DebugLog(const std::string& message);  // Debug logging function
    void DumpStateToDebug();

private:
    // State management
    ShopState currentState;
    DuckSkin currentSkin;
    std::vector<DuckSkin> unlockedSkins;
    float coins;
    int eggs;
    float exchangeRate;

    // Animation
    Texture2D coinAnimation;
    int currentCoinFrame;
    float frameTimer;
    const int COIN_FRAMES = 13;
    const float FRAME_DURATION = 0.1f;

    // Textures
    Texture2D background;
    Texture2D duckSkins[4];
    Texture2D gambleTextures[4]; // GM_duck4, GM_coin, GM_nothing, GM_egg
    Texture2D eggTexture;

    // Sounds
    Sound optSound;
    Sound transactionSound;
    Sound invalidSound;
    Sound gambleSound;

    // Exchange UI
    int exchangeAmount;
    void ExchangeAwarenessMessage();

    // Gamble UI
    int gambleResult;
    float gambleTimer;
    bool isGambling;
    void DrawGambleAwarenessQuote();


    // Private methods
    void UpdateCoinAnimation();
    void GenerateExchangeRate();
    void DrawCurrencyHeader();
    void DrawButton(const char* text, Rectangle bounds, bool active);
   
    
    // State-specific methods
    void UpdateMenu();
    void UpdateSkins();
    void UpdateExchange();
    void UpdateGamble();
    void DrawMenu();
    void DrawSkins();
    void DrawExchange();
    void DrawGamble();


   void DrawSkinAwarenessMessage();

    //debug 
    std::ofstream& GetDebugFile();
};