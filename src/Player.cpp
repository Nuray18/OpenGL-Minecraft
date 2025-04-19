#include "headers/Player.h"

Player::Player(vec3 startPosition)
{
}

void Player::jump()
{
}

void Player::update(float deltaTime)
{
    velocityY += gravity * deltaTime;
    position.y += velocityY * deltaTime;

    if (position.y <= 0.0f)
    {
        position.y = 0.0f;
        velocityY = 0.0f;
        isGrounded = true;
    }

    camera.position = position; // kamera oyuncuyu takip eder.
}

Camera &Player::getCamera()
{
    return camera;
}

vec3 Player::getPosition() const
{
    return position;
}