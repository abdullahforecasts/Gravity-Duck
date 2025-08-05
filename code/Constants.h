#pragma once
#include <string>

using namespace std;

namespace Constants {
    // Window
    const int WINDOW_WIDTH = 1120;
    const int WINDOW_HEIGHT = 1120;

    // Game objects
    const float DUCK_WIDTH = 60.0f;
    const float DUCK_HEIGHT = 60.0f;
    const float NAIL_DIM = 40.0f;

    const float SHOOTER_DIM = 80.0f;
    const bool DEBUG_MODE = true;

    const float WALL_DIM = 40.0f;
    const float EGG_DIM = 38.0f;
    const float RIGHTER_DIM = 50.0f;

    // killer objs
    const float GHOST_DIM = 60.0f;
    const float INSECT_DIM = 40.0f;
    const float BRICK_DIM = 40.0f;
    const float KILLER_OBJECT_SPEED = 120.0f;

    // Physics
    const float GRAVITY = 600.0f;
    const float MOVING_SPEED = 200.0f;  // Adjust this value as needed
    const float JUMP_FORCE = 600.0f;   // For gravity flipping

    const float BULLET_SPEED = 700.0f;
    const float RIGHTER_FORCE = 500.0f;
    const float COLLISION_THRESHOLD = 5.0f;
   
    // File paths
    const string LEVEL_FILES[] = {
        "assets/levels/level1.txt",
        "assets/levels/level2.txt",
        "assets/levels/level3.txt",
        "assets/levels/level4.txt",
        "assets/levels/level5.txt",
        "assets/levels/level6.txt",
        "assets/levels/level7.txt",
        "assets/levels/level8.txt",
        "assets/levels/level9.txt",
        "assets/levels/level10.txt",
        "assets/levels/level11.txt",
        
                      
    };
    const int NUM_LEVELS = 11;
}