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

    void update(vec3 movementDirection, float deltaTime, World &world);
    void jump();
    void getPlayerPos(); // this function is for debugging
    void toggleFlightMode();

    Camera &getCamera();

    vec3 getPosition() const;

    float speed = 10.0f;

private:
    void updateMovement(vec3 movementDirection, float deltaTime, World &world);
    void updateGravity(float deltaTime, World &world);
    void updateCamera();
    void updateCollider();

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