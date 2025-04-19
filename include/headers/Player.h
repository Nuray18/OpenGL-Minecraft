#pragma once
#include "headers/Camera.h"
#include "glm/glm.hpp"

using namespace glm;

class Player
{
public:
    Player(vec3 startPosition);
    void update(float deltaTime);
    void jump();
    Camera &getCamera();

    vec3 getPosition() const;

private:
    vec3 position;
    float velocityY;
    float gravity;
    float jumpStrength;
    bool isGrounded;

    Camera camera;
};