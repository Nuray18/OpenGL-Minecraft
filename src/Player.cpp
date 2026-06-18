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

void Player::updateMovement(vec3 movementDirection, float deltaTime)
{
    vec3 forwardDir;
    vec3 rightDir;

    if (flightMode)
    {
        forwardDir = normalize(camera.front);
        rightDir = normalize(camera.right);
    }
    else
    {
        forwardDir = normalize(
            vec3(camera.front.x, 0.0f, camera.front.z));

        rightDir = normalize(
            cross(forwardDir, vec3(0.0f, 1.0f, 0.0f)));
    }

    vec3 moveDir =
        forwardDir * movementDirection.z +
        rightDir * movementDirection.x;

    // HAREKET KISMI
    if (length(moveDir) > 0.0f)
    {
        moveDir = normalize(moveDir);
        position += moveDir * speed * deltaTime;
    }
}

void Player::updateGravity(float deltaTime)
{
    // YERÇEKİMİ
    if (!flightMode)
    {
        velocityY += gravity * deltaTime;
        position.y += velocityY * deltaTime;

        if (position.y <= 0.0f)
        {
            position.y = 0.0f;
            velocityY = 0.0f;
            isGrounded = true;
        }
    }
    else
    {
        velocityY = 0.0f;
    }
}

void Player::updateCamera()
{
    camera.position = position + vec3(0.0f, height, 0.0f);
}

void Player::update(vec3 movementDirection, float deltaTime)
{
    updateMovement(movementDirection, deltaTime);
    updateGravity(deltaTime);
    updateCamera();
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