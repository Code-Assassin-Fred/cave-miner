/// @file game.cpp
/// @brief CS 141 Group Project - Cave Miner Game with Rocket Collision & Damage

// For raylib graphics (raylib-cpp.hpp already includes raylib.h)
#include "raylib-cpp.hpp"

// C++ Libraries
#include <algorithm>
#include <iostream>
#include <random>

// User-Created Classes
#include "button.hpp"

// Targeted namespace inclusions
using std::mt19937;
using std::random_device;
using std::seed_seq;
using std::uniform_int_distribution;

// Represents an incoming rocket obstacle
struct Rocket
{
    raylib::Vector2 pos;
    float speed;
    float width;
    float height;
    bool useAltTexture;

    // Returns the rocket's bounding box for collision checks
    Rectangle GetHitbox() const
    {
        return Rectangle{pos.x, pos.y, width, height};
    }
};

// Resets a rocket to the right edge of the screen at a random vertical position
void ResetRocket(Rocket& rocket, int screenWidth, int screenHeight, mt19937& rng)
{
    const float minY = 60.0f;
    const float maxY = std::max(minY, static_cast<float>(screenHeight) - rocket.height - 10.0f);
    std::uniform_real_distribution<float> yDist(minY, maxY);

    rocket.pos = raylib::Vector2(static_cast<float>(screenWidth) + 20.0f, yDist(rng));
    rocket.useAltTexture = !rocket.useAltTexture; // Alternate between RocketA and RocketB
}

// Represents a falling rock obstacle from the 6x5 rock sprite sheet
struct FallingRock
{
    raylib::Vector2 pos;
    float speed;
    float size;
    int col;
    int row;

    // Returns the rock's bounding box for collision checks
    Rectangle GetHitbox() const
    {
        return Rectangle{pos.x, pos.y, size, size};
    }
};

// Resets a falling rock above the top of the screen at a random X position and sprite
void ResetRock(FallingRock& rock, int screenWidth, mt19937& rng)
{
    std::uniform_real_distribution<float> xDist(0.0f, static_cast<float>(screenWidth) - rock.size);
    uniform_int_distribution<int> colDist(0, 5);
    uniform_int_distribution<int> rowDist(0, 4);

    rock.pos = raylib::Vector2(xDist(rng), -rock.size);
    rock.col = colDist(rng);
    rock.row = rowDist(rng);
}

// Applies 1 point of damage if the player is not currently invulnerable
bool ApplyPlayerDamage(int& health, float& invincibleTimer, float invincibleDuration, bool& isGameOver)
{
    if (invincibleTimer > 0.0f)
    {
        return false;
    }

    health = std::max(0, health - 1);
    invincibleTimer = invincibleDuration;
    if (health <= 0)
    {
        isGameOver = true;
    }
    return true;
}

// Resets gameplay state for a fresh run
void ResetGameState(int& health, bool& isGameOver, bool& isPaused, float& invincibleTimer,
                    raylib::Vector2& minerPos, float ground, float& velocityY, bool& isJumping,
                    Rocket& rocket, FallingRock& rock, int screenWidth, int screenHeight, mt19937& rng)
{
    health = 3;
    isGameOver = false;
    isPaused = false;
    invincibleTimer = 0.0f;
    minerPos = raylib::Vector2(300.0f, ground);
    velocityY = 0.0f;
    isJumping = false;
    ResetRocket(rocket, screenWidth, screenHeight, rng);
    ResetRock(rock, screenWidth, rng);
}

int main()
{
    // Define base 16:9 resolution (matches zyBooks/cloud IDE optimal size)
    const int screenWidth = 960;
    const int screenHeight = 540;
    int currentScreen = 0; // 0 = Start Menu, 1 = Gameplay, 2 = Settings

    // ==========================================================================
    // OBJECT INSTANTIATION PHASE
    // Useful for objects that do not need to be created within the game loop.
    // ==========================================================================

    // Random number generator for sprite costumes and rocket spawn heights
    random_device rd;
    seed_seq seq{rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd()};
    mt19937 rng(seq);
    uniform_int_distribution<int> costumeDist(0, 15); // Sprite sheet rows

    // Initialize the window using the raylib-cpp wrapper
    raylib::Window window(screenWidth, screenHeight, "CS 141 Group Project");
    window.SetTargetFPS(60);

    // Instantiate non-dynamic Text objects
    raylib::Text titleText("Group Project Fall 2026!", 30, raylib::Color::RayWhite());

    // Game state flags
    bool exitGame = false;
    bool isPaused = false;
    bool showTutorial = false;
    bool isGameOver = false;

    // Load background and obstacle textures
    raylib::Texture2D cave("assets/Preview.png");
    raylib::Texture2D startCave("assets/1000054635.png");
    raylib::Texture2D rocketA("assets/RocketA.png");
    raylib::Texture2D rocketB("assets/RocketB.png");
    raylib::Texture2D rocks("assets/IMG_6232.png");
    const float rockSpriteWidth = static_cast<float>(rocks.width) / 6.0f;
    const float rockSpriteHeight = static_cast<float>(rocks.height) / 5.0f;

    // Load menu buttons
    Button startButton("assets/start button.png", {400, 150});
    Button exitButton("assets/exit button.png", {400, 250});
    Button optionButton("assets/option button.png", {400, 350});

    // Load player sprite sheet (5 columns of square frames)
    raylib::Texture2D minerSheet("assets/Miners_Square_Right.png");
    const float SpriteSize = static_cast<float>(minerSheet.width) / 5.0f;

    // Load heart image, replace its black background with transparency, and crop one heart
    raylib::Image heartImg("assets/Heart.png");
    heartImg.ColorReplace(BLACK, BLANK);
    raylib::Texture2D heart(heartImg);
    const int maxHealth = 3;
    int health = maxHealth;
    float invincibleTimer = 0.0f; // Prevents one hit from draining multiple hearts
    const float invincibleDuration = 1.0f;
    // Heart.png contains 3 hearts side-by-side, so crop only the first heart
    const raylib::Rectangle heartSource(0.0f, 70.0f, static_cast<float>(heart.width) / 3.0f, 70.0f);

    // Configure rocket dimensions and initial spawn
    const float rocketScale = 0.2f; // Scales 512x256 rocket down to ~102x51
    Rocket rocket{
        raylib::Vector2(static_cast<float>(screenWidth) + 20.0f, 0.0f),
        320.0f,
        static_cast<float>(rocketA.width) * rocketScale,
        static_cast<float>(rocketA.height) * rocketScale,
        false
    };
    ResetRocket(rocket, screenWidth, screenHeight, rng);

    // Configure falling rock obstacle and initial spawn
    FallingRock fallingRock{
        raylib::Vector2(0.0f, -48.0f),
        240.0f,
        48.0f,
        0,
        0
    };
    ResetRock(fallingRock, screenWidth, rng);

    // Load audio
    raylib::AudioDevice audiodevice;
    raylib::Sound jumpingSound("assets/audio/686523__xupr_e3__mixkit-arcade-game-jump-coin-216.wav");

    // Player position, movement, and animation state
    const float ground = static_cast<float>(screenHeight) - SpriteSize;
    raylib::Vector2 minerPos(300.0f, ground);
    float speed = 200.0f;

    int costumeRow = 0;
    int currentFrame = 0;
    float frameTimer = 0.0f;
    const float frameDuration = 0.12f;

    bool isMoving = false;
    bool facingLeft = true;

    // Jumping physics state
    bool isJumping = false;
    float velocityY = 0.0f;
    const float jumpSpeed = 650.0f;
    const float gravity = 1800.0f;

    // MAIN GAME LOOP
    while (!window.ShouldClose() && !exitGame)
    {
        // ==========================================
        // PHASE: UPDATE LOGIC & ACT ON USER INPUT
        // ==========================================
        float deltaTime = window.GetFrameTime();

        // Toggle fullscreen on F11 press from any screen
        if (raylib::Keyboard::IsKeyPressed(KEY_F11))
        {
            window.ToggleFullscreen();
        }

        if (currentScreen == 0)
        {
            // Main Menu button handling
            Vector2 mousePosition = GetMousePosition();
            bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

            if (startButton.isPressed(mousePosition, mousePressed))
            {
                std::cout << "Start Button Pressed\n";
                ResetGameState(health, isGameOver, isPaused, invincibleTimer,
                               minerPos, ground, velocityY, isJumping,
                               rocket, fallingRock, screenWidth, screenHeight, rng);
                showTutorial = true; // Show tutorial once when starting Level 1
                currentScreen = 1;
            }
            else if (optionButton.isPressed(mousePosition, mousePressed))
            {
                currentScreen = 2; // Switch to settings screen
            }
            else if (exitButton.isPressed(mousePosition, mousePressed))
            {
                exitGame = true; // Close game
            }
        }
        else if (currentScreen == 1)
        {
            // Dismiss tutorial overlay on Enter or Space
            if (showTutorial)
            {
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
                {
                    showTutorial = false;
                }
            }
            else if (isGameOver)
            {
                // Handle restart or return to menu on Game Over
                if (IsKeyPressed(KEY_R))
                {
                    ResetGameState(health, isGameOver, isPaused, invincibleTimer,
                                   minerPos, ground, velocityY, isJumping,
                                   rocket, fallingRock, screenWidth, screenHeight, rng);
                }
                else if (IsKeyPressed(KEY_M))
                {
                    ResetGameState(health, isGameOver, isPaused, invincibleTimer,
                                   minerPos, ground, velocityY, isJumping,
                                   rocket, fallingRock, screenWidth, screenHeight, rng);
                    currentScreen = 0;
                }
            }
            else
            {
                // Toggle pause with P during active gameplay
                if (IsKeyPressed(KEY_P))
                {
                    isPaused = !isPaused;
                }

                // Switch to a random costume with C
                if (raylib::Keyboard::IsKeyPressed(KEY_C))
                {
                    costumeRow = costumeDist(rng);
                }

                if (!isPaused)
                {
                    // Reset movement flag each frame
                    isMoving = false;

                    // Count down post-hit invulnerability timer
                    if (invincibleTimer > 0.0f)
                    {
                        invincibleTimer = std::max(0.0f, invincibleTimer - deltaTime);
                    }

                    // Jump on W key when grounded
                    if (raylib::Keyboard::IsKeyDown(KEY_W) && !isJumping)
                    {
                        velocityY -= jumpSpeed;
                        isJumping = true;
                        jumpingSound.Play();
                    }

                    // Move left
                    if (raylib::Keyboard::IsKeyDown(KEY_A))
                    {
                        minerPos.x -= speed * deltaTime;
                        isMoving = true;
                        facingLeft = true;
                    }

                    // Move right
                    if (raylib::Keyboard::IsKeyDown(KEY_D))
                    {
                        minerPos.x += speed * deltaTime;
                        isMoving = true;
                        facingLeft = false;
                    }

                    // Keep miner within horizontal screen bounds
                    minerPos.x = std::max(0.0f, std::min(minerPos.x, static_cast<float>(screenWidth) - SpriteSize));

                    // Apply jump velocity and gravity
                    if (isJumping)
                    {
                        minerPos.y += velocityY * deltaTime;
                        velocityY += gravity * deltaTime;
                    }

                    // Snap to ground upon landing
                    if (minerPos.y >= ground)
                    {
                        minerPos.y = ground;
                        isJumping = false;
                        velocityY = 0.0f;
                    }

                    // Advance walk animation frames while moving
                    if (isMoving)
                    {
                        frameTimer += deltaTime;
                        if (frameTimer >= frameDuration)
                        {
                            frameTimer = 0.0f;
                            currentFrame = (currentFrame + 1) % 5;
                        }
                    }
                    else
                    {
                        currentFrame = 0;
                        frameTimer = 0.0f;
                    }

                    // Move rocket from right to left across the screen
                    rocket.pos.x -= rocket.speed * deltaTime;

                    // Respawn rocket if it exits past the left edge of the screen
                    if (rocket.pos.x + rocket.width < 0.0f)
                    {
                        ResetRocket(rocket, screenWidth, screenHeight, rng);
                    }

                    // Check AABB collision between player and rocket
                    Rectangle playerRect = {minerPos.x, minerPos.y, SpriteSize, SpriteSize};
                    if (CheckCollisionRecs(playerRect, rocket.GetHitbox()) &&
                        ApplyPlayerDamage(health, invincibleTimer, invincibleDuration, isGameOver))
                    {
                        // Respawn rocket immediately so one hit never triggers repeatedly
                        ResetRocket(rocket, screenWidth, screenHeight, rng);
                    }

                    // Move falling rock from top to bottom
                    fallingRock.pos.y += fallingRock.speed * deltaTime;

                    // Check collision between player and falling rock
                    if (CheckCollisionRecs(playerRect, fallingRock.GetHitbox()) &&
                        ApplyPlayerDamage(health, invincibleTimer, invincibleDuration, isGameOver))
                    {
                        // Rock disappears on hit and respawns at the top
                        ResetRock(fallingRock, screenWidth, rng);
                    }
                    // If rock misses the player and hits the ground, it disappears and respawns at the top
                    else if (fallingRock.pos.y + fallingRock.size >= static_cast<float>(screenHeight))
                    {
                        ResetRock(fallingRock, screenWidth, rng);
                    }
                }
            }
        }
        else if (currentScreen == 2)
        {
            // Exit Settings screen back to Main Menu on C press
            if (raylib::Keyboard::IsKeyPressed(KEY_C))
            {
                currentScreen = 0;
            }
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
            int titleX = (window.GetWidth() - static_cast<int>(textDims.x)) / 2;
            int titleY = 20;
            titleText.Draw(titleX, titleY);

            startButton.Draw();
            optionButton.Draw();
            exitButton.Draw();
        }
        else if (currentScreen == 1)
        {
            cave.Draw();

            // Draw active rocket using alternating RocketA / RocketB textures
            const raylib::Texture2D& activeRocketTex = rocket.useAltTexture ? rocketB : rocketA;
            activeRocketTex.Draw(rocket.pos, 0.0f, rocketScale, raylib::Color::White());

            // Draw falling rock from the 6x5 sprite sheet
            raylib::Rectangle rockSource(
                static_cast<float>(fallingRock.col) * rockSpriteWidth,
                static_cast<float>(fallingRock.row) * rockSpriteHeight,
                rockSpriteWidth,
                rockSpriteHeight
            );
            raylib::Rectangle rockDest(fallingRock.pos.x, fallingRock.pos.y, fallingRock.size, fallingRock.size);
            rocks.Draw(rockSource, rockDest, Vector2{0.0f, 0.0f}, 0.0f, raylib::Color::White());

            // Draw miner sprite (flipped horizontally when facing left, blinking when hit)
            float drawWidth = facingLeft ? -SpriteSize : SpriteSize;
            float sourceX = static_cast<float>(currentFrame) * SpriteSize;
            float sourceY = static_cast<float>(costumeRow) * SpriteSize;
            raylib::Rectangle sourceRec(sourceX, sourceY, drawWidth, SpriteSize);

            raylib::Color playerTint = raylib::Color::White();
            if (invincibleTimer > 0.0f && (static_cast<int>(invincibleTimer * 10.0f) % 2 == 0))
            {
                playerTint = Fade(RED, 0.5f);
            }
            minerSheet.Draw(sourceRec, minerPos, playerTint);

            // Draw one cropped heart per remaining health point in the top-right corner
            for (int i = 0; i < health; ++i)
            {
                raylib::Rectangle heartDest(static_cast<float>(window.GetWidth()) - 150.0f + static_cast<float>(i) * 44.0f, 15.0f, 36.0f, 38.0f);
                heart.Draw(heartSource, heartDest, Vector2{0.0f, 0.0f}, 0.0f, raylib::Color::White());
            }

            DrawText("Press P to pause", 30, 20, 24, WHITE);

            // Draw tutorial overlay
            if (showTutorial)
            {
                DrawRectangle(0, 120, screenWidth, 240, Fade(BLACK, 0.85f));
                DrawText("HOW TO PLAY", 60, 145, 34, YELLOW);
                DrawText("Move: A / D   Jump: W   Switch Costume: C", 60, 200, 24, WHITE);
                DrawText("Dodge incoming rockets & falling rocks or lose a heart!", 60, 240, 24, WHITE);
                DrawText("Press ENTER or SPACE to start", 60, 300, 24, GREEN);
            }

            // Draw pause overlay
            if (isPaused && !showTutorial && !isGameOver)
            {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.5f));
                const char* pauseText = "GAME PAUSED";
                int fontSize = 40;
                int textWidth = MeasureText(pauseText, fontSize);
                DrawText(pauseText, (screenWidth - textWidth) / 2, screenHeight / 2 - 20, fontSize, DARKBLUE);
            }

            // Draw Game Over overlay
            if (isGameOver)
            {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f));
                const char* gameOverText = "GAME OVER";
                const char* promptText = "Press R to Restart or M for Main Menu";
                DrawText(gameOverText, (screenWidth - MeasureText(gameOverText, 50)) / 2, screenHeight / 2 - 50, 50, RED);
                DrawText(promptText, (screenWidth - MeasureText(promptText, 24)) / 2, screenHeight / 2 + 20, 24, WHITE);
            }
        }
        else if (currentScreen == 2)
        {
            ClearBackground(RAYWHITE);
            DrawText("This is the option/settings page.", 190, 200, 40, DARKBLUE);
            DrawText("Here the player can change their skin and put their name on a leaderboard", 50, 100, 22, DARKBLUE);
            DrawText("Press C to exit", 350, 300, 36, DARKBLUE);
        }

        window.EndDrawing();
    }

    return 0;
}
