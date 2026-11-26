//button class for start menu
#pragma once
#include <raylib.h>
#include "raylib-cpp.hpp"

class Button
{   
    public:
        Button(const char* imagePath, Vector2 imagePosition);
        ~Button();
        void Draw();
        bool isPressed(Vector2 mousePos, bool mousePressed); //boolean to check if menu button is pressed/clicked

    private:
        Texture2D texture;
        Vector2 position;
        //in raylib,vector2 is a data type representing a 2D
        //point or vector with x and y components
};