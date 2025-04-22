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
    flightMode = false; // ✨ default olarak normal mod
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

    if (!flightMode)
    {
        // First Person mod: gravity uygulanır
        velocityY += gravity * deltaTime;
        position.y += velocityY * deltaTime;

        if (position.y <= 0.0f)
        {
            position.y = 0.0f;
            velocityY = 0.0f;
            isGrounded = true;
        }

        // Kamera Y pozisyonunu oyuncunun başına göre ayarla
        camera.position.y = position.y + height;
    }
    else
    {
        // Free Fly mod: yerçekimi yok, kamera pozisyonu direkt alınır
        position = camera.position;
    }

    // X-Z pozisyonlarını kameradan al
    position.x = camera.position.x;
    position.z = camera.position.z;
}

Camera &Player::getCamera()
{
    return camera;
}

vec3 Player::getPosition() const
{
    return position;
}

void Player::toggleFlightMode()
{
    flightMode = !flightMode;
    if (flightMode)
    {
        // Uçuş moduna geçtiğinde vertical velocity sıfırlanır
        velocityY = 0.0f;
    }
}

void Player::getPlayerPos()
{
    float x = position.x;
    float y = position.y;
    float z = position.z;
    cout << x << " " << y << " " << z << endl;
}
