#include "headers/Player.h"

Player::Player(vec3 startPosition)
{
    position = startPosition;
    height = 1.75f;
    width = 0.3f;
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
    if (length(movementDirection) > 0.0f) // buradaki length functionu 0 dan farkli sayi var mi onu olcuyor.
    {
        vec3 moveDir = normalize(movementDirection);
        camera.processKeyboard(moveDir, deltaTime * speed);
    }

    if (!flightMode)
    {
        // First Person mod: gravity uygulanır
        velocityY += gravity * deltaTime;
        position.y += velocityY * deltaTime;

        // yer siniri
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

bool Player::checkCollision(vec3 newPosition, const World &world)
{
    for (float x = newPosition.x - width; x <= newPosition.x + width; x += 1.0f)
    {
        for (float y = newPosition.y; y <= newPosition.y + height; y += 1.0f)
        {
            for (float z = newPosition.z - width; z <= newPosition.z + width; z += 1.0f)
            {
                int blockX = static_cast<int>(floor(x));
                int blockY = static_cast<int>(floor(y));
                int blockZ = static_cast<int>(floor(z));

                BlockType block = world.getBlockGlobal(blockX, blockY, blockZ);
                if (block != BlockType::Air)
                {
                    return true;
                }
            }
        }
    }
    return false;
}