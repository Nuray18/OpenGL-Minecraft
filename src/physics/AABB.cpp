#include "headers/physics/AABB.h"

AABB::AABB()
{
    min = vec3(0.0f);
    max = vec3(0.0f);
}

AABB::AABB(const vec3 &_min, const vec3 &_max)
{
    min = _min;
    max = _max;
}

// updating every time when the player moves, so we can check if the player is colliding with the world or not.
void AABB::set(const vec3 &_min, const vec3 &_max)
{
    min = _min;
    max = _max;
}
// kesişiyor mu? veya çarpışıyor mu? yani eger carpisirsa intersects = true.
bool AABB::intersects(const AABB &other) const
{
    return (min.x <= other.max.x && max.x >= other.min.x) &&
           (min.y <= other.max.y && max.y >= other.min.y) &&
           (min.z <= other.max.z && max.z >= other.min.z);
}