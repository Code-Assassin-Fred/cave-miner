#include "button.hpp"

Button::Button(const char *imagePath, Vector2 imagePosition)
{
    texture = LoadTexture(imagePath); // textures are manually loaded and destroyed to free up memory
    position = imagePosition;
}

Button::~Button()
{
    UnloadTexture(texture);
}

void Button::Draw()
{
    DrawTextureV(texture, position, WHITE);
}

bool Button::isPressed(Vector2 mousePos, bool mousePressed) //check if mouse position colliudes with the rectangle that encloses the button
{
    Rectangle rect = {position.x, position.y, static_cast<float>(texture.width), static_cast<float>(texture.height)};
    if(CheckCollisionPointRec(mousePos,rect) && mousePressed)
    {
        return true;
    }
    return false;
}
