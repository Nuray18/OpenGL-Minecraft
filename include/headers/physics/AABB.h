#include "glm/glm.hpp"

using namespace glm;

class AABB
{
public:
    vec3 min;
    vec3 max;

    AABB();
    AABB(const vec3 &min, const vec3 &max);

    void set(const vec3 &min, const vec3 &max);
    bool intersects(const AABB &other) const;
};