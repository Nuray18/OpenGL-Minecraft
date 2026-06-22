#include "headers/Player.h"
#include "headers/World.h"
#include "headers/Camera.h"

Player::Player(vec3 startPosition)
{
    position = startPosition;
    camera.position = startPosition;
    height = 1.75f;
    width = 0.3f;
    velocityY = 0.0f;
    gravity = -9.81f;
    jumpStrength = 5.0f;
    isGrounded = true;
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

void Player::updateMovement(vec3 movementDirection, float deltaTime, World &world)
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
        vec3 oldPosition = position;

        // X hareketi
        position.x += moveDir.x * speed * deltaTime;

        updateCollider();

        if (world.checkCollision(collider))
        {
            position.x = oldPosition.x;
            updateCollider();
        }

        // Z hareketi
        position.z += moveDir.z * speed * deltaTime;

        updateCollider();

        if (world.checkCollision(collider))
        {
            position.z = oldPosition.z;
            updateCollider();
        }
    }
}

void Player::updateGravity(float deltaTime, World &world)
{
    // YERÇEKİMİ
    if (!flightMode)
    {
        float oldY = position.y;

        velocityY += gravity * deltaTime;
        position.y += velocityY * deltaTime;

        updateCollider();

        if (world.checkCollision(collider))
        {
            position.y = oldY;
            velocityY = 0.0f;
            updateCollider();
            isGrounded = true;
        }
        else
        {
            isGrounded = false;
        }
    }
    else
    {
        velocityY = 0.0f;
        isGrounded = false;
    }
}

void Player::updateCollider()
{
    // sol  = position.x - width
    // sağ  = position.x + width

    // alt  = position.y
    // üst  = position.y + height

    // ön   = position.z - width
    // arka = position.z + width
    vec3 min(
        position.x - width,
        position.y,
        position.z - width);

    vec3 max(
        position.x + width,
        position.y + height,
        position.z + width);

    collider.set(min, max);
}

void Player::updateCamera()
{
    camera.position = position + vec3(0.0f, height, 0.0f);
}

void Player::update(vec3 movementDirection, float deltaTime, World &world)
{
    updateMovement(movementDirection, deltaTime, world);
    updateGravity(deltaTime, world);
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

const AABB &Player::getCollider() const
{
    return collider;
}

/*
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
*/