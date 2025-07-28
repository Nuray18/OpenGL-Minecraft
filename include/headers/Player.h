#pragma once
#include "headers/Camera.h"
#include "headers/World.h"
#include "glm/glm.hpp"
#include <iostream>

using namespace glm;
using namespace std;

class Player
{
public:
    Player(vec3 startPosition);

    void update(vec3 movementDirection, float deltaTime);
    void jump();
    bool checkCollision(vec3 newPosition, const World &world);
    Camera &getCamera();
    void toggleFlightMode();

    vec3 getPosition() const;
    void getPlayerPos(); // this function is for debugging

    float speed = 10.0f;

private:
    vec3 position;
    float velocityY;
    float gravity;
    float jumpStrength;
    bool isGrounded;

    bool flightMode;

    float height; // player boyu.
    float width;  // player genisligi

    Camera camera;
};