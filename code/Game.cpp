#include "Game.h"
#include "Level.h"
#include "Constants.h"
#include "Duck.h"  // Add this include


Game* Game::instance = nullptr;

Game::Game() :
    currentLevel(0),
    isRunning(false),
    isPaused(false),
    showStartScreen(true),
    showWinScreen(false),
    fps(0),
    showDebugInfo(true),
    debugFile("debugfile.txt"),
    shop(nullptr)  // Initialize shop pointer to nullptr
{
    // Initialize Rectangle members properly
    startButton.x = 300;
    startButton.y = 709;
    startButton.width = 600;
    startButton.height = 150;

    restartButton.x = Constants::WINDOW_WIDTH / 2 - 100;
    restartButton.y = Constants::WINDOW_HEIGHT / 2 + 100;
    restartButton.width = 200;
    restartButton.height = 50;


     backToMenuButton = { (Constants::WINDOW_WIDTH / 2 - 100), Constants::WINDOW_HEIGHT / 2 + 160, 200, 50 };



    if (debugFile.is_open()) {
        debugFile << "Game constructor called" << std::endl;
    }


   
     

    // Create levels
    for (int i = 0; i < Constants::NUM_LEVELS; i++) {
        levels.push_back(new Level(Constants::LEVEL_FILES[i], i + 1));
        LogToFile("Created level " + std::to_string(i + 1));
    }

   
}

Game::~Game() {
    LogToFile("Game destructor called");
    for (auto level : levels) {
        if (level) {
            LogToFile("Deleting level " + to_string(level->GetLevelNumber()));
            level->Unload();
            delete level;
        }
    }
    levels.clear();

    // Clean up shop
    if (shop) {
        delete shop;
        shop = nullptr;
    }

    if (bgMusic.frameCount != 0) {
        StopMusicStream(bgMusic);
        UnloadMusicStream(bgMusic);
        LogToFile("Unloaded music");
    }

    if (debugFile.is_open()) {
        debugFile.close();
    }


    Duck::UnloadTextures();
   
    UnloadMusicStream(DuckBye);

    UnloadTexture(exitButtonTexture);

    UnloadTexture(startScreen);
    UnloadTexture(winScreen);

    delete [] instance;

}

void Game::LogToFile(const string& message) {
    if (debugFile.is_open()) {
        debugFile << message << endl;
    }
}

Game* Game::GetInstance() {
    if (!instance) {
        instance = new Game();
    }
    return instance;
}

void Game::Initialize() {
    LogToFile("Initializing game");
    InitWindow(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT, "Gravity Duck");
    InitAudioDevice();
    SetTargetFPS(60);
    SetWindowState(FLAG_VSYNC_HINT);

    if (!shop) {
        shop = new Shop();  // Create only after Raylib is initialized
    }

    Duck::LoadTextures();
    bgMusic = LoadMusicStream("assets/sounds/music.mp3");
    if (bgMusic.frameCount == 0) {
        LogToFile("ERROR: Failed to load music");
    }
    else {
        SetMusicVolume(bgMusic, 0.25f);
        PlayMusicStream(bgMusic);
        LogToFile("Loaded and playing music");
    }

    DuckBye = LoadMusicStream("assets/sounds/dv3.wav");  // make sure the path is correct
    SetMusicVolume(DuckBye, 1.0f);  // optional, max volume


    //exit button
    exitButton = { Constants::WINDOW_WIDTH - 50, 10, 40, 40 };
  
    

    startScreen = LoadTexture("assets/pngs/startwindow.png");
    if (startScreen.id == 0) {
        LogToFile("ERROR: Failed to load start screen texture");
    }

    winScreen = LoadTexture("assets/pngs/Win.png");
    if (winScreen.id == 0) {
        LogToFile("ERROR: Failed to load win screen texture");
    }

    isRunning = true;
    LogToFile("Game initialization complete");
}

void Game::Run() {
    LogToFile("Starting game loop");
    const float physicsStep = 1.0f / 60.0f;
    float accumulator = 0.0f;
    float fadeAlpha = 0.0f;
    bool isFading = false;

    while (!WindowShouldClose() && isRunning) {
        fps = GetFPS();
        UpdateMusicStream(bgMusic);
        
        if (hasPlayedDuckByeSound) {
            UpdateMusicStream(DuckBye);
        }

        //exit key
        if (IsKeyPressed(KEY_E) && !showStartScreen && !showWinScreen) {
            levels[currentLevel]->Unload();
            showStartScreen = true;
            RestartGame();
        }

       



             if (inShop) {
                shop->Update();
                shop->Draw();
                continue;
             }
       

            if (showStartScreen) {
                HandleStartScreenInput();
                DrawStartScreen();
                continue;
            }

            if (showWinScreen) {

                if (!hasPlayedDuckByeSound) {
                    PlayMusicStream(DuckBye);
                    SetMusicVolume(DuckBye, 2.0f);  // optional: set loudness
                    hasPlayedDuckByeSound = true;
                    LogToFile("DuckBye music started");
                }
                else {
                    UpdateMusicStream(DuckBye);  // ✅ keeps the music playing
                }


                HandleWinScreenInput();
                DrawWinScreen();
                continue;
            }

            float deltaTime = GetFrameTime();
            if (deltaTime > 0.25f) deltaTime = 0.25f;
            accumulator += deltaTime;

            HandleDebugInput();

            if (GetCurrentLevel() && GetCurrentLevel()->GetPlayer() && !GetCurrentLevel()->GetPlayer()->IsAlive()) {
                if (!isFading) {
                    LogToFile("Player died - starting fade");
                    isFading = true;
                    fadeAlpha = 0.0f;
                }

                fadeAlpha += deltaTime * 2.0f;
                if (fadeAlpha >= 1.0f) {
                    LogToFile("Fade complete - restarting level");
                    RestartLevel();
                    isFading = false;
                    fadeAlpha = 0.0f;
                }
            }

            while (accumulator >= physicsStep && !isPaused && !isFading) {
                if (GetCurrentLevel()) {
                    GetCurrentLevel()->Update(physicsStep);
                }
                accumulator -= physicsStep;
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);

            if (GetCurrentLevel()) {
                if (!isFading) {
                    GetCurrentLevel()->Draw();
                }
                else {
                    GetCurrentLevel()->Draw();
                    DrawRectangle(0, 0, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT,
                        Color{ 0, 0, 0, static_cast<unsigned char>(fadeAlpha * 255) });
                }
            }

            if (isPaused) {
                DrawRectangle(0, 0, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT, { 0,0,0,180 });
                DrawText("PAUSED", Constants::WINDOW_WIDTH / 2 - MeasureText("PAUSED", 50) / 2,
                    Constants::WINDOW_HEIGHT / 2 - 25, 50, WHITE);
            }

            if (showDebugInfo) {
                DrawDebugInfo();
            }

            EndDrawing();

            if (!isPaused && !isFading && GetCurrentLevel() && GetCurrentLevel()->IsCompleted()) {
                LogToFile("Level " + to_string(currentLevel + 1) + " completed");
                NextLevel();
            }

           
    }
    LogToFile("Game loop ended");
}

void Game::NextLevel() {
    if (!GetCurrentLevel()) return;

   // winSoundPlayed = false; 

    LogToFile("Moving to next level from " + to_string(currentLevel + 1));
    GetCurrentLevel()->Unload();
    
    // Ensure we don't go out of bounds
    if (currentLevel + 1 < Constants::NUM_LEVELS) {
        currentLevel++;
        LogToFile("Loading level " + to_string(currentLevel + 1));
        GetCurrentLevel()->Load();
    } else {
        LogToFile("All levels completed - showing win screen");
        showWinScreen = true;
        // Don't increment currentLevel beyond bounds
    }
}

void Game::PreviousLevel() {
    if (!GetCurrentLevel() || currentLevel <= 0) return;

    LogToFile("Moving to previous level from " + to_string(currentLevel + 1));
    GetCurrentLevel()->Unload();
    currentLevel--;
    LogToFile("Loading level " + to_string(currentLevel + 1));
    GetCurrentLevel()->Load();
}

void Game::RestartLevel() {
    if (!GetCurrentLevel()) return;

    LogToFile("Restarting level " + to_string(currentLevel + 1));

    // Get current level file path
    string levelFile = Constants::LEVEL_FILES[currentLevel];
    int levelNum = currentLevel + 1;

    // Create new level before deleting old one
    Level* newLevel = new Level(levelFile, levelNum);

    // Delete old level
    Level* oldLevel = levels[currentLevel];
    levels[currentLevel] = newLevel;
    delete oldLevel;

    // Load new level
    levels[currentLevel]->Load();
}

void Game::RestartGame() {
    LogToFile("Restarting entire game");

    // Unload current level if exists
    if (GetCurrentLevel()) {
        GetCurrentLevel()->Unload();
    }

    // Recreate all levels
    for (auto& level : levels) {
        delete level;
    }
    levels.clear();

    for (int i = 0; i < Constants::NUM_LEVELS; i++) {
        levels.push_back(new Level(Constants::LEVEL_FILES[i], i + 1));
    }

    currentLevel = 0;
    levels[currentLevel]->Load();
    showWinScreen = false;
}

void Game::Cleanup() {
    LogToFile("Cleaning up game resources");
    for (auto level : levels) {
        if (level) {
            level->Unload();
        }
    }

    UnloadTexture(startScreen);
    UnloadTexture(winScreen);
    if (bgMusic.frameCount != 0) {
        StopMusicStream(bgMusic);
        UnloadMusicStream(bgMusic);
    }
    CloseAudioDevice();
    CloseWindow();
}

void Game::DrawStartScreen() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawTexture(startScreen, 0, 0, WHITE);

    // Animated blinking border
    float time = GetTime();
    float alpha = (sinf(time * 4.0f) * 0.5f + 0.5f) * 255;  // Blinks 4 times per second
    float thickness = 2 + (sinf(time * 2.0f) * 1.0f);       // Pulses between 1 and 3

    Color animatedColor = { 255, 165, 0, static_cast<unsigned char>(alpha) }; // ORANGE with animated alpha

    // Draw animated border
    DrawRectangleLinesEx(startButton, thickness, animatedColor);


    EndDrawing();
}

void Game::DrawWinScreen() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawTexture(winScreen, 0, 0, WHITE);

   

   
        DrawRectangleLinesEx(restartButton, 2, RED);
        DrawText("RESTART GAME", restartButton.x + 10, restartButton.y + 15, 20, BLACK);

        DrawRectangleLinesEx(backToMenuButton, 2, RED);  // Outline box
        DrawText("BACK TO MENU", backToMenuButton.x + 10, backToMenuButton.y + 15, 20, BLACK);
    

   

    EndDrawing();
}

void Game::HandleStartScreenInput() {
    if (IsKeyPressed(KEY_F3)) {
        showDebugInfo = !showDebugInfo;
        LogToFile("Toggled debug info: " + string(showDebugInfo ? "ON" : "OFF"));
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();
        if (CheckCollisionPointRec(mousePos, startButton)) {
            LogToFile("Start game button pressed");
            showStartScreen = false;
            EnterShop();  // Enter shop instead of directly loading level
        }
    }
}

void Game::HandleWinScreenInput() {
    if (IsKeyPressed(KEY_F3)) {
        showDebugInfo = !showDebugInfo;
        LogToFile("Toggled debug info: " + string(showDebugInfo ? "ON" : "OFF"));
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();

        // Handle Restart Game Button
        if (CheckCollisionPointRec(mousePos, restartButton)) {
            LogToFile("Restart game button pressed");
            if (hasPlayedDuckByeSound) {
                StopMusicStream(DuckBye);
                hasPlayedDuckByeSound = false;
            }

            RestartGame();
        }

        // Handle Back to Menu Button
        if (CheckCollisionPointRec(mousePos, backToMenuButton)) {
            // Return to main menu
            if (hasPlayedDuckByeSound) {
                StopMusicStream(DuckBye);
                hasPlayedDuckByeSound = false;
            }

            RestartGame();
            showWinScreen = false;
            showStartScreen = true;
            inShop = false;  // Ensure shop is closed
        }
    }

}

void Game::HandleDebugInput() {
    if (IsKeyPressed(KEY_F3)) {
        showDebugInfo = !showDebugInfo;
        LogToFile("Toggled debug info: " + string(showDebugInfo ? "ON" : "OFF"));
    }

    if (IsKeyPressed(KEY_W)) {
        LogToFile("DEBUG: Next level command");
        NextLevel();
    }
    if (IsKeyPressed(KEY_O)) {
        LogToFile("DEBUG: Previous level command");
        PreviousLevel();
    }

    if (IsKeyPressed(KEY_P)) {
        TogglePause();
        LogToFile("DEBUG: Pause toggled");
    }
    if (IsKeyPressed(KEY_R)) {
        LogToFile("DEBUG: Restart level command");
        RestartLevel();
    }
}

void Game::DrawDebugInfo() {
    DrawRectangle(10, 10, 300, 120, Color{ 0, 0, 0, 180 });

    DrawText(TextFormat("FPS: %d", (int)fps), 20, 20, 20, GREEN);
    DrawText(TextFormat("Level: %d/%d", currentLevel + 1, Constants::NUM_LEVELS), 20, 45, 20, WHITE);

    if (GetCurrentLevel() && GetCurrentLevel()->GetPlayer()) {
        auto player = GetCurrentLevel()->GetPlayer();
        DrawText(TextFormat("Pos: (%.1f, %.1f)", player->GetX(), player->GetY()), 20, 70, 20, WHITE);
        DrawText(TextFormat("Alive: %s", player->IsAlive() ? "YES" : "NO"), 20, 95, 20,
            player->IsAlive() ? GREEN : RED);
    }

   // DrawText("F3: Toggle Debug Info", 20, 120, 20, YELLOW);
}

void Game::EnterShop() {
    inShop = true;
    isPaused = true;
    shop->LoadState();  // Load shop state when entering
}

void Game::ExitShop() {
    inShop = false;
    isPaused = false;
    shop->SaveState();  // Save shop state when exiting
}

Level* Game::GetCurrentLevel() const {
    if (currentLevel >= 0 && currentLevel < (int)levels.size())
        return levels[currentLevel];
    return nullptr;
}

void Game::SetRunning(bool value) {
    isRunning = value;
}

void Game::PlayIntroSlides(int totalSlides) {
   vector<Texture2D> slideTextures;
   vector<Music> slideAudios;

    // Load textures and audio for each slide
    for (int i = 1; i <= totalSlides; i++) {
        string imagePath = "assets/slides/s" + to_string(i) + ".png";
        string audioPath = "assets/slides/sa" + to_string(i) + ".wav";

        Texture2D slideTex = LoadTexture(imagePath.c_str());
        Music slideMusic = LoadMusicStream(audioPath.c_str());

        slideTextures.push_back(slideTex);
        slideAudios.push_back(slideMusic);
    }

    int currentSlide = 0;
    bool playingSlides = true;
    bool audioPlayed = false;

    while (playingSlides && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexture(slideTextures[currentSlide], 0, 0, WHITE);
        EndDrawing();

        // Play the audio for the current slide once
        if (!audioPlayed) {
            PlayMusicStream(slideAudios[currentSlide]);
            audioPlayed = true;
        }

        UpdateMusicStream(slideAudios[currentSlide]);

        if (IsKeyPressed(KEY_N)) {
            StopMusicStream(slideAudios[currentSlide]);
            currentSlide++;
            audioPlayed = false;

            if (currentSlide >= totalSlides) {
                playingSlides = false;
            }
        }

        if (IsKeyPressed(KEY_M)) {
            StopMusicStream(slideAudios[currentSlide]);
            playingSlides = false;
        }
    }

    // Cleanup
    for (int i = 0; i < totalSlides; i++) {
        UnloadTexture(slideTextures[i]);
        UnloadMusicStream(slideAudios[i]);
    }

   
}


