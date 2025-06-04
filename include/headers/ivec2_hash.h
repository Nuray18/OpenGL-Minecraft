// include/headers/ivec2_hash.h
#pragma once
#include <glm/glm.hpp>
#include <functional>

using namespace glm;

namespace std
{
    template <>
    struct hash<ivec2>
    {
        size_t operator()(const ivec2 &v) const noexcept
        {
            size_t h1 = hash<int>()(v.x);
            size_t h2 = hash<int>()(v.y);
            return h1 ^ (h2 << 1); // basit hash kombinasyonu
        }
    };
}
