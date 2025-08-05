#include "Shop.h"
#include "Constants.h"
#include <algorithm>
#include <random>
#include"Game.h"
#include <iomanip>
#include "Level.h"

Shop::Shop() :
    currentState(MENU),
    currentSkin(DEFAULT),
    coins(0),
    eggs(0),
    currentCoinFrame(0),
    frameTimer(0),
    exchangeAmount(1),
    gambleResult(-1),
    isGambling(false),
    exchangeRate(4.0f) {

    DebugLog("Shop constructor started");
    DebugLog("Initializing shop with default values:");
    DebugLog("- State: MENU");
    DebugLog("- Skin: DEFAULT");
    DebugLog("- Coins: 0");
    DebugLog("- Eggs: 0");

    // Load textures
    DebugLog("Loading background texture...");
    background = LoadTexture("assets/pngs/background1.jpg");
    if (background.id == 0) DebugLog("ERROR: Failed to load background texture");
    else DebugLog("Background texture loaded successfully");

    DebugLog("Loading coin animation texture...");
    coinAnimation = LoadTexture("assets/pngs/coin_animation.png");
    if (coinAnimation.id == 0) DebugLog("ERROR: Failed to load coin animation");
    else DebugLog("Coin animation loaded successfully");

    DebugLog("Loading egg texture...");
    eggTexture = LoadTexture("assets/pngs/egg.png");
    if (eggTexture.id == 0) DebugLog("ERROR: Failed to load egg texture");
    else DebugLog("Egg texture loaded successfully");

    DebugLog("All base textures loaded");

    // Load duck skins
    DebugLog("Loading duck skins...");
    duckSkins[0] = LoadTexture("assets/pngs/duck1.png");
    if (duckSkins[0].id == 0) DebugLog("ERROR: Failed to load duck1.png");

    duckSkins[1] = LoadTexture("assets/pngs/duck2.png");
    if (duckSkins[1].id == 0) DebugLog("ERROR: Failed to load duck2.png");

    duckSkins[2] = LoadTexture("assets/pngs/duck3.png");
    if (duckSkins[2].id == 0) DebugLog("ERROR: Failed to load duck3.png");

    duckSkins[3] = LoadTexture("assets/pngs/duck4.png");
    if (duckSkins[3].id == 0) DebugLog("ERROR: Failed to load duck4.png");

    DebugLog("Duck skins loading completed");

    // Load gamble textures
    DebugLog("Loading gamble textures...");
    gambleTextures[0] = LoadTexture("assets/pngs/GM_duck4.png");
    if (gambleTextures[0].id == 0) DebugLog("ERROR: Failed to load GM_duck4.png");

    gambleTextures[1] = LoadTexture("assets/pngs/GM_coin.png");
    if (gambleTextures[1].id == 0) DebugLog("ERROR: Failed to load GM_coin.png");

    gambleTextures[2] = LoadTexture("assets/pngs/GM_egg.png");
    if (gambleTextures[2].id == 0) DebugLog("ERROR: Failed to load GM_egg.png");

    gambleTextures[3] = LoadTexture("assets/pngs/GM_nothing.png");
    if (gambleTextures[3].id == 0) DebugLog("ERROR: Failed to load GM_nothing.png");

    DebugLog("Gamble textures loading completed");

    // Load sounds
    DebugLog("Loading sounds...");
    optSound = LoadSound("assets/sounds/opt.mp3");
    if (optSound.frameCount == 0) DebugLog("ERROR: Failed to load opt.mp3");

    transactionSound = LoadSound("assets/sounds/transaction.mp3");
    if (transactionSound.frameCount == 0) DebugLog("ERROR: Failed to load transaction.mp3");

    invalidSound = LoadSound("assets/sounds/invalid.mp3");
    if (invalidSound.frameCount == 0) DebugLog("ERROR: Failed to load invalid.mp3");

    gambleSound = LoadSound("assets/sounds/gamble.mp3");
    if (gambleSound.frameCount == 0) DebugLog("ERROR: Failed to load gamble.mp3");

    DebugLog("Sound loading completed");

    DebugLog("Loading saved state...");
    LoadState();
    DebugLog("Generating initial exchange rate...");
    GenerateExchangeRate();
    DebugLog("Shop initialization completed");
    DumpStateToDebug();
}

Shop::~Shop() {
    DebugLog("Shop destructor started");
    DebugLog("Unloading textures...");

    // Unload textures
    DebugLog("Unloading background texture...");
    UnloadTexture(background);

    DebugLog("Unloading coin animation...");
    UnloadTexture(coinAnimation);

    DebugLog("Unloading egg texture...");
    UnloadTexture(eggTexture);

    for (int i = 0; i < 4; i++) {
        DebugLog("Unloading duck skin " + std::to_string(i + 1) + "...");
        UnloadTexture(duckSkins[i]);

        DebugLog("Unloading gamble texture " + std::to_string(i + 1) + "...");
        UnloadTexture(gambleTextures[i]);
    }

    // Unload sounds
    DebugLog("Unloading sounds...");
    UnloadSound(optSound);
    UnloadSound(transactionSound);
    UnloadSound(invalidSound);
    UnloadSound(gambleSound);

    DebugLog("Shop cleanup completed");
    DumpStateToDebug();
}

void Shop::Update() {
    UpdateCoinAnimation();

    if (IsKeyPressed(KEY_ESCAPE)) {
        GoToMenu();
    }

    switch (currentState) {
    case MENU:      UpdateMenu(); break;
    case SKINS:     UpdateSkins(); break;
    case EXCHANGE:  UpdateExchange(); break;
    case GAMBLE:    UpdateGamble(); break;
    }
}

void Shop::Draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawTexture(background, 0, 0, WHITE);
    DrawCurrencyHeader();

    switch (currentState) {
    case MENU:      DrawMenu(); break;
    case SKINS:     DrawSkins(); break;
    case EXCHANGE:  DrawExchange(); break;
    case GAMBLE:    DrawGamble(); break;
    }

    // Back button
    if (currentState != MENU) {
        DrawButton("BACK", { 20, Constants::WINDOW_HEIGHT - 70, 100, 50 }, true);
    }

    EndDrawing();
}

void Shop::SaveState() {
    std::ofstream file("GameState.txt");
    if (file.is_open()) {
        file << eggs << "\n";
        file << coins << "\n";
        file << static_cast<int>(currentSkin) << "\n";

        for (auto skin : unlockedSkins) {
            file << static_cast<int>(skin) << " ";
        }
        file.close();
    }
}

void Shop::LoadState() {
    std::ifstream file("GameState.txt");
    if (file.is_open()) {
        file >> eggs;
        file >> coins;

        int skin;
        file >> skin;
        currentSkin = static_cast<DuckSkin>(skin);

        unlockedSkins.clear();
        while (file >> skin) {
            unlockedSkins.push_back(static_cast<DuckSkin>(skin));
        }
        file.close();
    }
    else {
        // Default state
        eggs = 0;
        coins = 0;
        currentSkin = DEFAULT;
        unlockedSkins = { DEFAULT };
        SaveState();
    }
}

void Shop::GoToMenu() { currentState = MENU; }
void Shop::GoToSkins() { currentState = SKINS; }
void Shop::GoToGamble() { currentState = GAMBLE; }
void Shop::GoToExchange() {
    currentState = EXCHANGE;
    GenerateExchangeRate();
}

// PRIVATE METHODS //

void Shop::UpdateCoinAnimation() {
    frameTimer += GetFrameTime();
    if (frameTimer >= FRAME_DURATION) {
        frameTimer = 0;
        currentCoinFrame = (currentCoinFrame + 1) % COIN_FRAMES;
    }
}

void Shop::GenerateExchangeRate() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(3.5, 5.99);
    exchangeRate = static_cast<float>(dist(gen));
}

void Shop::DrawCurrencyHeader() {
    // Egg count
    DrawTexture(eggTexture, 20, 20, WHITE);
    DrawText(TextFormat("%d", eggs), 70, 30, 30, WHITE);

    // Coin animation and count
    Rectangle coinFrame = { static_cast<float>(currentCoinFrame * 39), 0, 39, 38 };
    DrawTextureRec(coinAnimation, coinFrame, { Constants::WINDOW_WIDTH - 180, 20 }, WHITE);
    DrawText(TextFormat("%.2f", coins), Constants::WINDOW_WIDTH - 130, 30, 30, WHITE);
}

void Shop::DrawButton(const char* text, Rectangle bounds, bool active) {
    Color bgColor = active ? GREEN : GRAY;
    DrawRectangleRec(bounds, bgColor);
    DrawRectangleLinesEx(bounds, 2, DARKGRAY);

    int textWidth = MeasureText(text, 20);
    DrawText(text, bounds.x + bounds.width / 2 - textWidth / 2, bounds.y + bounds.height / 2 - 10, 20, WHITE);
}

void Shop::UpdateMenu() {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();

        float buttonY = 200;
        const float buttonHeight = 60;
        const float buttonSpacing = 20;

        // Check button clicks
        buttonY = 200;
        if (CheckCollisionPointRec(mousePos, { Constants::WINDOW_WIDTH / 2 - 100, buttonY, 200, buttonHeight })) {
            // Play game

            Game::GetInstance()->ExitShop(); // Exit shop menu
            Game::GetInstance()->PlayIntroSlides(9);
            Game::GetInstance()->GetCurrentLevel()->Load(); // Start first level
        }
        buttonY += buttonHeight + buttonSpacing;
        if (CheckCollisionPointRec(mousePos, { Constants::WINDOW_WIDTH / 2 - 100, buttonY, 200, buttonHeight })) {
            GoToSkins();
        }
        buttonY += buttonHeight + buttonSpacing;
        if (CheckCollisionPointRec(mousePos, { Constants::WINDOW_WIDTH / 2 - 100, buttonY, 200, buttonHeight })) {
            GoToExchange();
        }
        buttonY += buttonHeight + buttonSpacing;
        if (CheckCollisionPointRec(mousePos, { Constants::WINDOW_WIDTH / 2 - 100, buttonY, 200, buttonHeight })) {
            GoToGamble();
        }
        buttonY += buttonHeight + buttonSpacing;
        if (CheckCollisionPointRec(mousePos, { Constants::WINDOW_WIDTH / 2 - 100, buttonY, 200, buttonHeight })) {
            // Quit game

            Game::GetInstance()->SetRunning(false);
        }

        // Back button
        if (CheckCollisionPointRec(mousePos, { 20, Constants::WINDOW_HEIGHT - 70, 100, 50 })) {
            GoToMenu();
        }
    }
}

void Shop::DrawMenu() {
    // Title
    DrawText("MAIN MENU", Constants::WINDOW_WIDTH / 2 - MeasureText("MAIN MENU", 40) / 2, 100, 40, WHITE);

    // Menu options
    float buttonY = 200;
    const float buttonHeight = 60;
    const float buttonSpacing = 20;

    DrawButton("PLAY", { Constants::WINDOW_WIDTH / 2 - 100, buttonY, 200, buttonHeight }, true);
    buttonY += buttonHeight + buttonSpacing;

    DrawButton("SKINS", { Constants::WINDOW_WIDTH / 2 - 100, buttonY, 200, buttonHeight }, true);
    buttonY += buttonHeight + buttonSpacing;

    DrawButton("EXCHANGE", { Constants::WINDOW_WIDTH / 2 - 100, buttonY, 200, buttonHeight }, true);
    buttonY += buttonHeight + buttonSpacing;

    DrawButton("GAMBLE", { Constants::WINDOW_WIDTH / 2 - 100, buttonY, 200, buttonHeight }, true);
    buttonY += buttonHeight + buttonSpacing;

    DrawButton("QUIT", { Constants::WINDOW_WIDTH / 2 - 100, buttonY, 200, buttonHeight }, true);
}

void Shop::UpdateSkins() {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();

        // Check skin selection
        for (int i = 0; i < 4; i++) {
            Rectangle skinRect = { 100 + i * 150, 200, 100, 100 };
            if (CheckCollisionPointRec(mousePos, skinRect)) {
                DuckSkin skin = static_cast<DuckSkin>(i + 1);

                // Check if unlocked
                if (std::find(unlockedSkins.begin(), unlockedSkins.end(), skin) != unlockedSkins.end()) {
                    currentSkin = skin;
                    PlaySound(optSound);
                    SaveState();
                }
                else {
                    // Try to buy if not unlocked
                    if (skin == BUYABLE1 && coins >= 150.99f) {
                        coins -= 150.99f;
                        unlockedSkins.push_back(BUYABLE1);
                        currentSkin = BUYABLE1;
                        PlaySound(transactionSound);
                        SaveState();
                    }
                    else if (skin == BUYABLE2 && coins >= 290.62f) {
                        coins -= 290.62f;
                        unlockedSkins.push_back(BUYABLE2);
                        currentSkin = BUYABLE2;
                        PlaySound(transactionSound);
                        SaveState();
                    }
                    else {
                        PlaySound(invalidSound);
                    }
                }
            }
        }

        // Back button
        if (CheckCollisionPointRec(mousePos, { 20, Constants::WINDOW_HEIGHT - 70, 100, 50 })) {
            GoToMenu();
        }
    }
}

void Shop::DrawSkins() {
    DrawText("SKIN SELECTION", Constants::WINDOW_WIDTH / 2 - MeasureText("SKIN SELECTION", 40) / 2, 100, 40, WHITE);

    for (int i = 0; i < 4; i++) {
        DuckSkin skin = static_cast<DuckSkin>(i + 1);
        bool isUnlocked = std::find(unlockedSkins.begin(), unlockedSkins.end(), skin) != unlockedSkins.end();

        // Background
        Color bgColor = LIGHTGRAY;
        if (currentSkin == skin) bgColor = GREEN;
        else if (!isUnlocked) bgColor = RED;

        Rectangle skinRect = { 100 + i * 150, 200, 100, 100 };
        DrawRectangleRec(skinRect, bgColor);
        DrawRectangleLinesEx(skinRect, 2, DARKGRAY);

        // Skin texture
      /*  DrawTexturePro(duckSkins[i],
            { 0, 0, (float)duckSkins[i].width, (float)duckSkins[i].height },
            { 100 + i * 150 + 20, 210, 60, 60 },
            { 0, 0 }, 0, WHITE);

      */
        Rectangle source = { 0, 0, (float)duckSkins[i].width, (float)duckSkins[i].height };
        Rectangle dest = { (float)(100 + i * 150 + 20), 210, 60, 60 };
        Vector2 origin = { 0, 0 };

        DrawTexturePro(duckSkins[i], source, dest, origin, 0, WHITE);


        // Status text
        const char* status = "";
        if (skin == currentSkin) status = "SELECTED";
        else if (!isUnlocked) {
            if (skin == BUYABLE1) status = "150.99";
            else if (skin == BUYABLE2) status = "290.62";
            else if (skin == GAMBLE_SKIN) status = "GAMBLE";
        }
        else {
            status = "OWNED";
        }

        DrawText(status, 100 + i * 150 + 50 - MeasureText(status, 20) / 2, 320, 20, WHITE);
    }

    DrawSkinAwarenessMessage();
}

void Shop::UpdateExchange() {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();

        // Increase amount (aligned with visual "+" button)
        if (CheckCollisionPointRec(mousePos, { Constants::WINDOW_WIDTH / 2 + 50, 280, 30, 30 })) {
            exchangeAmount++;
            if (exchangeAmount > eggs) {
                exchangeAmount = eggs;
                PlaySound(invalidSound);
            }
        }
        // Decrease amount (aligned with visual "-" button)
        else if (CheckCollisionPointRec(mousePos, { Constants::WINDOW_WIDTH / 2 - 90, 280, 30, 30 })) {
            exchangeAmount--;
            if (exchangeAmount < 1) exchangeAmount = 1;
        }
        // Exchange button
        else if (CheckCollisionPointRec(mousePos, { Constants::WINDOW_WIDTH / 2 - 100, 380, 200, 50 })) {
            if (exchangeAmount <= eggs) {
                eggs -= exchangeAmount;
                coins += exchangeAmount * exchangeRate;
                PlaySound(transactionSound);
                SaveState();
                GenerateExchangeRate();
                exchangeAmount = 1;
            }
            else {
                PlaySound(invalidSound);
            }
        }

        // Back button (unchanged)
        if (CheckCollisionPointRec(mousePos, { 20, Constants::WINDOW_HEIGHT - 70, 100, 50 })) {
            GoToMenu();
        }
    }
}

void Shop::DrawExchange() {
    // --- TITLE: QUACKEN MARKET with Neon Background ---
    const char* title = "QUACKEN MARKET";
    int titleFontSize = 40;
    int titleWidth = MeasureText(title, titleFontSize);
    Rectangle titleBox = {
        Constants::WINDOW_WIDTH / 2 - titleWidth / 2 - 20,
        80,
        static_cast<float>(titleWidth + 40),
        50
    };

    // Draw background
    DrawRectangleRounded(titleBox, 0.2f, 10, Fade(YELLOW, 0.25f));
    // Draw title text
    DrawText(title, Constants::WINDOW_WIDTH / 2 - titleWidth / 2, 90, titleFontSize, DARKGREEN);

    // Neon dots around the box
    float t = GetTime();
    Color neonColors[] = { GREEN, LIME, YELLOW, SKYBLUE };
    for (int i = 0; i < 4; i++) {
        float angle = t * 2 + i * PI / 2;
        float x = titleBox.x + titleBox.width / 2 + cosf(angle) * (titleBox.width / 2 + 10);
        float y = titleBox.y + titleBox.height / 2 + sinf(angle) * (titleBox.height / 2 + 10);
        DrawCircle(x, y, 4, Fade(neonColors[i % 4], 0.3f));
        DrawCircle(x, y, 2, neonColors[i % 4]);
    }

    // --- EXCHANGE RATE TEXT ---
    DrawText(TextFormat("EXCHANGE RATE: %.2f coins per egg", exchangeRate),
        Constants::WINDOW_WIDTH / 2 - MeasureText(TextFormat("EXCHANGE RATE: %.2f coins per egg", exchangeRate), 30) / 2,
        160, 30, WHITE);

    // --- VISUALIZATION ---
    DrawTexture(eggTexture, Constants::WINDOW_WIDTH / 2 - 150, 220, WHITE);
    DrawText("->", Constants::WINDOW_WIDTH / 2 - 30, 230, 40, WHITE);
    Rectangle coinFrame = { static_cast<float>(currentCoinFrame * 39), 0, 39, 38 };
    DrawTextureRec(coinAnimation, coinFrame, { Constants::WINDOW_WIDTH / 2 + 50, 220 }, WHITE);

    // --- AMOUNT SELECTION ---
    DrawText(TextFormat("%d", exchangeAmount), Constants::WINDOW_WIDTH / 2 - 30, 260, 30, WHITE);

    // Move both buttons slightly down, and "+" slightly left
    DrawButton("+", { Constants::WINDOW_WIDTH / 2 + 50, 280, 30, 30 }, exchangeAmount < eggs);
    DrawButton("-", { Constants::WINDOW_WIDTH / 2 - 90, 280, 30, 30 }, exchangeAmount > 1);

    // --- ESTIMATED COINS (moved lower) ---
    DrawText(TextFormat("= %.2f coins", exchangeAmount * exchangeRate),
        Constants::WINDOW_WIDTH / 2 - MeasureText(TextFormat("= %.2f coins", exchangeAmount * exchangeRate), 30) / 2,
        330, 30, WHITE);

    // --- EXCHANGE BUTTON ---
    DrawButton("EXCHANGE", { Constants::WINDOW_WIDTH / 2 - 100, 380, 200, 50 }, exchangeAmount <= eggs);

    ExchangeAwarenessMessage();
}

void Shop::UpdateGamble() {
    if (isGambling) {
        gambleTimer -= GetFrameTime();
        if (gambleTimer <= 0) {
            isGambling = false;

            // Apply reward based on final gambleResult
            switch (gambleResult) {
            case 0: // Duck skin
                if (std::find(unlockedSkins.begin(), unlockedSkins.end(), GAMBLE_SKIN) == unlockedSkins.end()) {
                    unlockedSkins.push_back(GAMBLE_SKIN);
                    DebugLog("Gamble result: Skin unlocked");
                }
                else {
                    DebugLog("Gamble result: Skin already unlocked");
                }
                break;
            case 1: // 30 coins
                coins += 30;
                DebugLog("Gamble result: Won 30 coins");
                break;
            case 2: // 1 egg
                AddEgg();
                DebugLog("Gamble result: Won 1 egg");
                break;
            case 3: // Nothing
                DebugLog("Gamble result: Nothing won");
                break;
            }

            SaveState();
        }

        return;
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();

        // Gamble button clicked
        if (CheckCollisionPointRec(mousePos, { Constants::WINDOW_WIDTH / 2 - 100, 350, 200, 50 })) {
            if (eggs >= 3) {
                eggs -= 3;
                isGambling = true;
                gambleTimer = 3.0f; // animate for full 3 seconds
                PlaySound(gambleSound);

                // Determine result
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dist(0, 99);
                int roll = dist(gen);

                if (roll < 2) {
                    gambleResult = 0; // 2% chance → Skin
                }
                else if (roll < 22) {
                    gambleResult = 1; // Next 20% → Coins (2–21)
                }
                else if (roll < 47) {
                    gambleResult = 2; // Next 25% → Egg (22–46)
                }
                else {
                    gambleResult = 3; // Remaining 53% → Nothing (47–99)
                }


                DebugLog("Gamble initiated. Result index: " + std::to_string(gambleResult));
                SaveState();
            }
            else {
                PlaySound(invalidSound);
            }
        }

        // Back button
        if (CheckCollisionPointRec(mousePos, { 20, Constants::WINDOW_HEIGHT - 70, 100, 50 })) {
            GoToMenu();
        }
    }
}

void Shop::DrawGamble() {
    // --- Animated Title ---
   
   // Constants
    const Rectangle titleBox = { Constants::WINDOW_WIDTH / 2 - 150, 50, 300, 70 };
    const char* titleText = "QUACKPOT";

    // Draw title background
    DrawRectangleRounded(titleBox, 0.3f, 20, DARKBLUE);

    // Draw text centered
    int textWidth = MeasureText(titleText, 40);
    DrawText(titleText, titleBox.x + titleBox.width / 2 - textWidth / 2, titleBox.y + 15, 40, GOLD);

    // Bubble positions relative to title box
    Vector2 bubbleOffsets[] = {
        { -40, -10 }, { -40, 30 }, { -40, 70 },               // Left side
        { titleBox.width + 10, -10 }, { titleBox.width + 10, 30 }, { titleBox.width + 10, 70 }, // Right side
        { 30, -30 }, { titleBox.width / 2 - 10, -40 }, { titleBox.width - 50, -30 },            // Top
        { 30, titleBox.height + 10 }, { titleBox.width / 2 - 10, titleBox.height + 20 }, { titleBox.width - 50, titleBox.height + 10 } // Bottom
    };

    // Bubble color palette
    Color bubbleColors[] = { RED, GREEN, ORANGE, GOLD, SKYBLUE, PURPLE };

    // Draw glow bubbles
    float glowSize = 14;
    for (int i = 0; i < 12; i++) {
        Vector2 pos = { titleBox.x + bubbleOffsets[i].x, titleBox.y + bubbleOffsets[i].y };
        Color base = bubbleColors[i % (sizeof(bubbleColors) / sizeof(Color))];

        // Outer glow
        DrawCircleV(pos, glowSize, Fade(base, 0.2f));
        DrawCircleV(pos, glowSize * 0.6f, Fade(base, 0.4f));
        DrawCircleV(pos, glowSize * 0.3f, WHITE);
    }










    // --- Result Display Area ---
    Rectangle resultArea = { Constants::WINDOW_WIDTH / 2 - 50, 200, 100, 100 };
    DrawRectangleRec(resultArea, DARKGRAY);

    if (isGambling) {
        int frame = static_cast<int>(GetTime() * 10) % 4;
        DrawTexturePro(gambleTextures[frame],
            { 0, 0, 38, 38 },
            { resultArea.x + resultArea.width / 2 - 19, resultArea.y + resultArea.height / 2 - 19, 38, 38 },
            { 0, 0 }, 0, WHITE);
    }
    else {
        DrawTexturePro(gambleTextures[gambleResult],
            { 0, 0, 38, 38 },
            { resultArea.x + resultArea.width / 2 - 19, resultArea.y + resultArea.height / 2 - 19, 38, 38 },
            { 0, 0 }, 0, WHITE);
    }

    // --- Gamble Button (keep green button logic as-is) ---
    DrawButton("GAMBLE", { Constants::WINDOW_WIDTH / 2 - 100, 350, 200, 50 }, eggs >= 3 && !isGambling);
    DrawText("[3 EGGS EACH GAMBLE] ", Constants::WINDOW_WIDTH / 2 - 180, 410, 30, MAROON);

    // --- Prize Odds Display ---
    DrawText("PRIZES ", Constants::WINDOW_WIDTH / 2 - 80, 450, 30, WHITE);
    DrawText("2% - Duck Skin", Constants::WINDOW_WIDTH / 2 - 80, 490, 25, DARKGREEN);
    DrawText("20% - 30 coins", Constants::WINDOW_WIDTH / 2 - 80, 520, 25, GOLD);
    DrawText("25% - 1 egg", Constants::WINDOW_WIDTH / 2 - 80, 550, 25, ORANGE);
    DrawText("53% - Nothing", Constants::WINDOW_WIDTH / 2 - 80, 580, 25, RED);



    this->DrawGambleAwarenessQuote();



}

void Shop::DrawGambleAwarenessQuote() {
    const float fontSize = 20;
    const float lineSpacing = 4;
    const float boxPadding = 20;
    const float boxSpacing = 25;
    const float boxWidth = Constants::WINDOW_WIDTH * 0.8f;
    const float boxHeight = 100;

    const float startX = Constants::WINDOW_WIDTH / 2 - boxWidth / 2;
    const float startY = 620;

    const char* quotes[] = {
        "You spin for luck.\nBut ducks are gambled away for profit.\nNo second chances. No respawns.",
        "Go read: # Plucked Alive: The Hidden Suffering Behind Down Production.#\nThousands of ducks.\nTorn feathers. Silent screams.\nAnd people still call it luxury.",
        "Look up: #The Massacre of the Male Ducklings.#\nBorn male? Then born to die.\nGassed, shredded, or bagged alive.",
        "You are spinning for skins.\nThey are spinning blades for feathers.\nStill feel lucky?"
    };

    Color boxColor = Fade(RED, 0.5f); // semi-transparent red
    Color textColor = YELLOW;

    for (int i = 0; i < 4; i++) {
        Rectangle box = {
            startX,
            startY + i * (boxHeight + boxSpacing),
            boxWidth,
            boxHeight
        };

        DrawRectangleRounded(box, 0.15f, 10, boxColor);

        // Split into lines
        std::vector<std::string> lines;
        const char* text = quotes[i];
        const char* lineStart = text;

        while (*lineStart) {
            const char* lineEnd = strchr(lineStart, '\n');
            if (lineEnd) {
                lines.emplace_back(lineStart, lineEnd);
                lineStart = lineEnd + 1;
            }
            else {
                lines.emplace_back(lineStart);
                break;
            }
        }

        // Compute total height
        int totalTextHeight = static_cast<int>(lines.size() * (fontSize + lineSpacing)) - lineSpacing;
        float y = box.y + box.height / 2 - totalTextHeight / 2;

        // Draw each line centered
        for (const std::string& line : lines) {
            int textWidth = MeasureText(line.c_str(), fontSize);
            float x = box.x + box.width / 2 - textWidth / 2;
            DrawText(line.c_str(), static_cast<int>(x), static_cast<int>(y), fontSize, textColor);
            y += fontSize + lineSpacing;
        }
    }
}

void Shop::ExchangeAwarenessMessage() {
    const char* messages[] = {
        "Some ducks never grow old enough to have skin.\nThe males are culled.\nThe rest are plucked until they bleed.",
        "Too male? You're killed.\nToo useful? You're plucked.\nEither way, you lose your skin.",
        "Male ducklings are tossed out on day one.\nThe lucky ones grow up\n just to be stripped for style",
        "Curious what real ducks go through for -style-?\nSearch: #Inside Hatcheries and Down Factories  L214 & PETA Exposes#"
    };
   
    int fontSize = 20;
    int boxWidth = Constants::WINDOW_WIDTH - 100;
    int boxHeight = 90;
    int boxX = 50;
    int boxYStart = 450;
    int spacing = 20;

    for (int i = 0; i < 4; ++i) {
        float boxY = static_cast<float>(boxYStart + i * (boxHeight + spacing));
        Rectangle box = { static_cast<float>(boxX), boxY, static_cast<float>(boxWidth), static_cast<float>(boxHeight) };

        Color boxColor = Fade(PINK, 0.3f);
        Color textColor = DARKPURPLE;

        // Background box
        DrawRectangleRounded(box, 0.15f, 10, boxColor);

        // Multiline centered text
        int lineHeight = fontSize + 4;
        float yCursor = box.y + 10;

        const char* text = messages[i];
        const char* lineStart = text;

        while (*lineStart) {
            const char* lineEnd = strchr(lineStart, '\n');
            std::string line;
            if (lineEnd) {
                line = std::string(lineStart, lineEnd);
                lineStart = lineEnd + 1;
            }
            else {
                line = std::string(lineStart);
                lineStart += strlen(lineStart);
            }

            int textWidth = MeasureText(line.c_str(), fontSize);
            DrawText(line.c_str(), box.x + box.width / 2 - textWidth / 2, static_cast<int>(yCursor), fontSize, textColor);
            yCursor += lineHeight;
        }
    }
}

void Shop::DrawSkinAwarenessMessage() {
    const float fontSize = 20;
    const float lineSpacing = 4;
    const float boxPadding = 20;
    const float boxSpacing = 20;
    const float boxWidth = Constants::WINDOW_WIDTH * 0.75f;
    const float boxHeight = 100;

    const float startX = Constants::WINDOW_WIDTH / 2 - boxWidth / 2;
    const float startY = 360; // Just below status text at y = 320

    const char* quotes[] = {
        "In games, skins are cosmetic.\nIn life, they are taken feather by feather, scream by scream.",
        "Male ducklings donot get skins.\nThey get sorted, crushed, and forgotten.",
        "Read: # Inside the Down Industry: Live Plucked for Fashion # (PETA).\nDucks and geese have feathers torn from their skin while fully conscious.\nFor coats, cushions, and comforters.",
        "Look up: # Millions of Male Ducklings Are Ground Up Alive # (L214 Investigation).\nNot profitable? Then not allowed to live."
    };

    Color boxColor = Fade(YELLOW, 0.5f);  
    Color textColor = DARKBLUE;

    for (int i = 0; i < 4; i++) {
        Rectangle box = {
            startX,
            startY + i * (boxHeight + boxSpacing),
            boxWidth,
            boxHeight
        };

        DrawRectangleRounded(box, 0.15f, 10, boxColor);

        // Split each quote into lines
        std::vector<std::string> lines;
        const char* text = quotes[i];
        const char* lineStart = text;

        while (*lineStart) {
            const char* lineEnd = strchr(lineStart, '\n');
            if (lineEnd) {
                lines.emplace_back(lineStart, lineEnd);
                lineStart = lineEnd + 1;
            }
            else {
                lines.emplace_back(lineStart);
                break;
            }
        }

        // Compute vertical centering
        int totalTextHeight = static_cast<int>(lines.size() * (fontSize + lineSpacing)) - lineSpacing;
        float y = box.y + box.height / 2 - totalTextHeight / 2;

        // Draw each line centered
        for (const std::string& line : lines) {
            int textWidth = MeasureText(line.c_str(), fontSize);
            float x = box.x + box.width / 2 - textWidth / 2;
            DrawText(line.c_str(), static_cast<int>(x), static_cast<int>(y), fontSize, textColor);
            y += fontSize + lineSpacing;
        }
    }
}

void Shop::DebugLog(const std::string& message) {
    std::ofstream& debugFile = GetDebugFile();
    if (debugFile.is_open()) {
        debugFile << "[SHOP] " << message << std::endl;
    }
}

void Shop::DumpStateToDebug() {
    std::ofstream& debugFile = GetDebugFile();
    if (debugFile.is_open()) {
        debugFile << "\n===== SHOP STATE DUMP =====" << std::endl;
        debugFile << "[SHOP] Current State: " << static_cast<int>(currentState) << std::endl;
        debugFile << "[SHOP] Current Skin: " << static_cast<int>(currentSkin) << std::endl;
        debugFile << "[SHOP] Coins: " << std::fixed << std::setprecision(2) << coins << std::endl;
        debugFile << "[SHOP] Eggs: " << eggs << std::endl;
        debugFile << "[SHOP] Exchange Rate: " << exchangeRate << std::endl;
        debugFile << "[SHOP] Unlocked Skins: ";
        for (auto skin : unlockedSkins) {
            debugFile << static_cast<int>(skin) << " ";
        }
        debugFile << std::endl;
        debugFile << "[SHOP] Is Gambling: " << (isGambling ? "Yes" : "No") << std::endl;
        debugFile << "[SHOP] Gamble Result: " << gambleResult << std::endl;
        debugFile << "[SHOP] Gamble Timer: " << gambleTimer << std::endl;
        debugFile << "==========================\n" << std::endl;
    }
}

std::ofstream& Shop::GetDebugFile() {
    // Get reference to Game's debug file
    static std::ofstream debugFile("debugfile.txt", std::ios::app);
    return debugFile;
}