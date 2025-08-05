#include "Game.h"

int main() {
    
    Game* game = Game::GetInstance();
    game->Initialize();
    game->Run();
    game->Cleanup();
    delete game;

    return 0;
}












#include "raylib.h"
#include <vector>

using std::vector;

struct Box {
    Rectangle rect;
    Color color;
};

//int main1() {
//    const int drawWidth = 1120;
//    const int panelWidth = 200;
//    const int screenWidth = drawWidth + panelWidth;
//    const int screenHeight = 1120;
//
//    InitWindow(screenWidth, screenHeight, "Click-to-Place Box with Info Panel");
//    SetTargetFPS(60);
//
//    // --- Load Background Image ---
//    Texture2D bg = LoadTexture("assets/pngs/k.png");
//
//    const int boxSize = 40;
//    Color activeColor = GREEN;
//    vector<Box> boxes;
//
//    Vector2 lastClick = { -1, -1 };
//
//    while (!WindowShouldClose()) {
//        // Change color
//        if (IsKeyPressed(KEY_N)) activeColor = ORANGE;
//        if (IsKeyPressed(KEY_B)) activeColor = BLUE;
//        if (IsKeyPressed(KEY_Y)) activeColor = PURPLE;
//
//        // Handle click
//        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//            Vector2 mouse = GetMousePosition();
//
//            if (mouse.x < drawWidth) { // Prevent clicking in panel area
//                float x = ((int)mouse.x / boxSize) * boxSize;
//                float y = ((int)mouse.y / boxSize) * boxSize;
//
//                Box newBox = { {x, y, (float)boxSize, (float)boxSize}, activeColor };
//                boxes.push_back(newBox);
//                lastClick = { x, y };
//            }
//        }
//
//        // --- Draw ---
//        BeginDrawing();
//        ClearBackground(RAYWHITE);
//
//        // Draw background image scaled to 1120x1120
//        Rectangle source = { 0, 0, (float)bg.width, (float)bg.height };
//        Rectangle dest = { 0, 0, (float)drawWidth, (float)screenHeight };
//        Vector2 origin = { 0, 0 };
//        DrawTexturePro(bg, source, dest, origin, 0.0f, WHITE);
//
//        // Draw all placed boxes
//        for (const Box& b : boxes) {
//            DrawRectangleRec(b.rect, b.color);
//        }
//
//        // Info Panel
//        int panelX = drawWidth + 10;
//        DrawLine(drawWidth, 0, drawWidth, screenHeight, DARKGRAY);
//        DrawText("INFO PANEL", panelX, 10, 20, BLACK);
//
//        DrawText("Controls:", panelX, 40, 18, DARKGRAY);
//        DrawText("B - Brown", panelX, 60, 18, BROWN);
//        DrawText("Y - Yellow", panelX, 80, 18, YELLOW);
//        DrawText("N - Black", panelX, 100, 18, BLACK);
//
//        DrawText("Click to place box", panelX, 130, 18, DARKGRAY);
//        DrawText("Box Size: 40x40", panelX, 160, 18, BLACK);
//
//        // Last click coordinates
//        if (lastClick.x >= 0) {
//            DrawText("Last Box at:", panelX, 200, 18, DARKGRAY);
//            DrawText(TextFormat("X: %.0f", lastClick.x), panelX, 220, 18, BLACK);
//            DrawText(TextFormat("Y: %.0f", lastClick.y), panelX, 240, 18, BLACK);
//        }
//
//        // Show active color
//        DrawText("Active Color:", panelX, 280, 18, DARKGRAY);
//        DrawRectangle(panelX, 310, 40, 40, activeColor);
//        DrawRectangleLines(panelX, 310, 40, 40, BLACK);
//
//        EndDrawing();
//    }
//
//    // Clean up
//    UnloadTexture(bg);
//    CloseWindow();
//    return 0;
//}
