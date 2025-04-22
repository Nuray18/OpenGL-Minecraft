#include "headers/Player.h"

Player::Player(vec3 startPosition)
{
    position = startPosition;
    height = 1.75f;
    velocityY = 0.0f;
    gravity = -9.81f;
    jumpStrength = 5.0f;
    isGrounded = true;
    camera = startPosition;
}

void Player::jump()
{
    if (isGrounded)
    {
        velocityY = jumpStrength;
        isGrounded = false;
    }
}

void Player::update(vec3 movementDirection, float deltaTime)
{
    // 1️⃣ WASD yön hareketi varsa uygula
    if (length(movementDirection) > 0.0f)
    {
        vec3 moveDir = normalize(movementDirection);
        camera.processKeyboard(moveDir, deltaTime);
    }

    // 2️⃣ X-Z pozisyonlarını kameradan al
    position.x = camera.position.x;
    position.z = camera.position.z;

    // 3️⃣ Yerçekimi işle
    velocityY += gravity * deltaTime;
    position.y += velocityY * deltaTime;

    if (position.y <= 0.0f)
    {
        position.y = 0.0f;
        velocityY = 0.0f;
        isGrounded = true;
    }

    // 4️⃣ Kamera Y pozisyonunu oyuncuya göre ayarla
    camera.position.y = position.y + height;
}

Camera &Player::getCamera()
{
    return camera;
}

vec3 Player::getPosition() const
{
    return position;
}