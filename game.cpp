/// @file game.cpp
/// @brief CS 141 Group Project Base Template

// For raylib graphics
#include "raylib-cpp.hpp"

// C++ Libraries
#include <raylib.h>
#include "button.hpp"
#include <iostream>

int main() 
{
    // Define base 16:9 resolution (matches zyBooks/cloud IDE optimal size)
    const int screenWidth = 960;
    const int screenHeight = 540;
    int currentScreen = 0;
    float sw = static_cast<float>(screenWidth);
    float sh = static_cast<float>(screenHeight);
    float scaleFactor = 1.0f;

    // Initialize the window using the raylib-cpp wrapper
    raylib::Window window(screenWidth, screenHeight, "CS 141 Group Project");
    window.SetTargetFPS(60);

    // Instantiate non-dynamic Text objects
    raylib::Text titleText("Group Project Fall 2026!", 30, raylib::Color::RayWhite());

    // Exit
    bool exit = false;

    raylib::Texture2D startCave("assets/1000054635.png");

    // load menu buttons
    Button startButton("assets/start button.png", {400, 150});
    Button optionButton("assets/option button.png", {400, 350});
    Button exitButton("assets/exit button.png", {400, 250});

    // MAIN GAME LOOP
    while (!window.ShouldClose() && exit == false)
    {
        // MAIN GAME MENU
        if (currentScreen == 0)
        {
            Vector2 mousePosition = GetMousePosition();
            bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

            if (startButton.isPressed(mousePosition, mousePressed))
            {
                std::cout << "Start Button Pressed";
                currentScreen = 1; // screen switches to game
            }

            if (optionButton.isPressed(mousePosition, mousePressed))
            {
                currentScreen = 2; // screen switches to settings
            }

            if (exitButton.isPressed(mousePosition, mousePressed))
            {
                exit = true; // game screen closes
            }
        }

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

        if (currentScreen == 0)
        {
            startCave.Draw(0, 0);
            raylib::Vector2 textDims = titleText.MeasureEx();
            int titleX = (window.GetWidth() - textDims.x) / 2;
            int titleY = 20;
            titleText.Draw(titleX, titleY);

            startButton.Draw();
            optionButton.Draw();
            exitButton.Draw();
        }

        if (currentScreen == 2)
        {
            ClearBackground(RAYWHITE);
            DrawText("This is the option/settings page.", 190, 200, 40, DARKBLUE);
            DrawText("Here the player can change their skin and put their name on a leaderboard", 50, 100, 40, DARKBLUE);
            DrawText("Press C to exit", 400, 300, 40, DARKBLUE);

            if (raylib::Keyboard::IsKeyPressed(KEY_C))
            {
                currentScreen = 0;
            }
        }

        window.EndDrawing();
    }

    return 0;
}
