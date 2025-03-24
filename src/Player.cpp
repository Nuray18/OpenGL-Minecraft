#include <glad/glad.h>
#include "headers/Player.h"
#include <iostream>

Player::Player(int _x, int _y, int _width, int _height)
{
    x = _x;
    y = _y;

    width = _width;
    height = _height;

    velocityX = 0;
    velocityY = 0;

    walkSpeed = 100.0f;
}

void Player::handleInput(SDL_Event &event)
{
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
    {
        bool isPressed = event.type == SDL_KEYDOWN;
        float speed = isPressed ? walkSpeed : 0.0f;

        switch (event.key.keysym.sym)
        {
        case SDLK_w:
            velocityY = -speed;
            break;

        case SDLK_s:
            velocityY = speed;
            break;

        case SDLK_a:
            velocityX = -speed;
            break;

        case SDLK_d:
            velocityX = speed;
            break;
        }
    }
}

void Player::update(float deltaTime)
{
    move(deltaTime); // Hareketi güncelleme
}

void Player::move(float deltaTime)
{
    x += velocityX * deltaTime;
    y += velocityY * deltaTime;
}

void Player::render(int screenHeight)
{
    std::cout << "Rendering player at (" << x << ", " << y << ")" << std::endl;

    // glLoadIdentity();            // Önce matris sıfırlanmalı
    // glColor3f(0.0f, 0.0f, 1.0f); // Mavi renk

    // glBegin(GL_QUADS);
    // glVertex2f(x, y);                  // Sol alt
    // glVertex2f(x + width, y);          // Sağ alt
    // glVertex2f(x + width, y + height); // Sağ üst
    // glVertex2f(x, y + height);         // Sol üst
    // glEnd();
}