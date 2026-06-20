#pragma once
#include "headers/Camera.h"
#include "physics/AABB.h"

#include "glm/glm.hpp"

#include <iostream>

using namespace glm;
using namespace std;

class World; // Forward declaration of the World class

class Player
{
public:
    Player(vec3 startPosition);

    void update(vec3 movementDirection, float deltaTime);
    void jump();
    void getPlayerPos(); // this function is for debugging

    Camera &getCamera();

    vec3 getPosition() const;
    void toggleFlightMode();

    float speed = 10.0f;

private:
    void updateMovement(vec3 movementDirection, float deltaTime);
    void updateGravity(float deltaTime);
    void updateCamera();

    vec3 position;

    AABB collider; // Playerin collision box'u

    float velocityY;
    float gravity;
    float jumpStrength;

    bool isGrounded;
    bool flightMode;

    float height; // player boyu.
    float width;  // player genisligi

    Camera camera; // playerin camerasi onun icin ozel olan yani playere ait olan camera olusturuyoruz
};