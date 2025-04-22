#pragma once
#include "headers/Camera.h"
#include "glm/glm.hpp"

using namespace glm;

class Player
{
public:
    Player(vec3 startPosition);

    void update(vec3 movementDirection, float deltaTime);
    void jump();
    Camera &getCamera();

    vec3 getPosition() const;

    float speed = 1.0f;

private:
    vec3 position;
    float velocityY;
    float gravity;
    float jumpStrength;
    bool isGrounded;

    float height; // player boyu.

    Camera camera;
};