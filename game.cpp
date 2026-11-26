/// @file game.cpp
/// @brief CS 141 Group Project Base Template

// For raylib graphics
#include "raylib-cpp.hpp"

// C++ Libraries
#include <random>
#include <raylib.h>
#include "button.hpp"
#include <iostream>

// Targeted namespace inclusions
using std::string;
using std::random_device;
using std::mt19937;
using std::seed_seq;
using std::uniform_int_distribution;

int main() 
{
    // Define base 16:9 resolution (matches zyBooks/cloud IDE optimal size)
    const int screenWidth = 960;
    const int screenHeight = 540;
    int currentScreen = 0;
    float sw = static_cast<float>(screenWidth);
    float sh = static_cast<float>(screenHeight);
    float scaleFactor = 1.0f;

    // Range with a sequence
    random_device rd;
    seed_seq seq{rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd()};
    mt19937 rng(seq);
    uniform_int_distribution<int> costumeDist(0, 15); // sprite sheet rows

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

    // Load Sprite
    raylib::Texture2D minerSheet("assets/Miners_Square_Right.png");
    const float SpriteSize = static_cast<float>(minerSheet.width) / 5.0f;

    // sounds
    raylib::AudioDevice audiodevice;
    raylib::Sound jumpingSound("assets/audio/686523__xupr_e3__mixkit-arcade-game-jump-coin-216.wav");

    // creating animation state and position
    raylib::Vector2 minerPos(300.0f, screenHeight - SpriteSize);
    float speed = 200.0f;

    int costumeRow = 0;
    int currentFrame = 0;
    float frameTimer = 0.0f;

    // checks the direction of the sprite
    bool isMoving = false;
    bool facingLeft = true;

    // jumping check
    bool isJumping = false;
    float velocityY = 0.0f;
    float jumpSpeed = 650.0f;
    float gravity = 1800.0f;
    const float ground = screenHeight - SpriteSize;

    // MAIN GAME LOOP
    while (!window.ShouldClose() && exit == false)
    {
        float deltaTime = window.GetFrameTime();

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

        // moves to a random costume
        if (raylib::Keyboard::IsKeyPressed(KEY_C))
        {
            costumeRow = costumeDist(rng);
        }

        // reset movement flag everytime
        isMoving = false;

        // Toggle fullscreen on F11 press
        if (raylib::Keyboard::IsKeyPressed(KEY_F11)) 
        {
            window.ToggleFullscreen();
        }

        // jumping W key
        if (raylib::Keyboard::IsKeyDown(KEY_W) && !isJumping)
        {
            velocityY -= jumpSpeed;
            isJumping = true;
            jumpingSound.Play();
        }

        if (raylib::Keyboard::IsKeyDown(KEY_S))
        {
            minerPos.y += speed * deltaTime;
            isMoving = true;
        }

        if (raylib::Keyboard::IsKeyDown(KEY_A))
        {
            minerPos.x -= speed * deltaTime;
            isMoving = true;
            facingLeft = true;
        }

        if (raylib::Keyboard::IsKeyDown(KEY_D))
        {
            minerPos.x += speed * deltaTime;
            isMoving = true;
            facingLeft = false;
        }

        // jumping movement
        if (isJumping)
        {
            minerPos.y += velocityY * deltaTime;
            velocityY += gravity * deltaTime;
        }

        // landing
        if (minerPos.y >= ground)
        {
            minerPos.y = ground;
            isJumping = false;
            velocityY = 0.0f;
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

        if (currentScreen == 1)
        {
            float drawWidth;
            if (facingLeft == true)
            {
                drawWidth = -SpriteSize;
            }
            else
            {
                drawWidth = SpriteSize;
            }

            float sourceX = currentFrame * SpriteSize;
            float sourceY = costumeRow * SpriteSize;

            raylib::Rectangle sourceRec(sourceX, sourceY, drawWidth, SpriteSize);
            minerSheet.Draw(sourceRec, minerPos);
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
