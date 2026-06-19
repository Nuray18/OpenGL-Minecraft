#include "headers/physics/collision.h"

bool Collision::CheckCollision(const AABB &a, const AABB &b)
{
    // a -> this, b -> other yani player(a) b ile cakisiyor mu ona bakilir.
    return a.intersects(b);
}