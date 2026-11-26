/// @file game.cpp
/// @brief CS 141 Group Project Base Template

// For raylib graphics
#include "raylib-cpp.hpp"

// C++ Libraries
#include <iostream>

int main() 
{
    // Define base 16:9 resolution (matches zyBooks/cloud IDE optimal size)
    const int screenWidth = 960;
    const int screenHeight = 540;

    // Initialize the window using the raylib-cpp wrapper
    raylib::Window window(screenWidth, screenHeight, "CS 141 Group Project");
    window.SetTargetFPS(60);

    // Instantiate non-dynamic Text objects
    raylib::Text titleText("Group Project Fall 2026!", 30, raylib::Color::RayWhite());

    // MAIN GAME LOOP
    while (!window.ShouldClose())
    {
        // Toggle fullscreen on F11 press
        if (raylib::Keyboard::IsKeyPressed(KEY_F11)) 
        {
            window.ToggleFullscreen();
        }

        // ==========================================
        // PHASE: DRAWING
        // ==========================================
        window.BeginDrawing();
        window.ClearBackground(raylib::Color::Black());

        // MeasureEx() returns a Vector2 containing both width (.x) and height (.y)
        raylib::Vector2 textDims = titleText.MeasureEx();
        int titleX = (window.GetWidth() - textDims.x) / 2; 
        int titleY = 20; 
        titleText.Draw(titleX, titleY);

        window.EndDrawing();
    }

    return 0;
}
