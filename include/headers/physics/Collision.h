#pragma once

#include "headers/physics/AABB.h"

class Collision
{
public:
    static bool CheckCollision(const AABB &a, const AABB &b);
};