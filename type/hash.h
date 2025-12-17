#pragma once
#include <glm/vec3.hpp>
#include <functional>
// Custom specialization of std::hash can be injected in namespace std.
template<>
struct std::hash<glm::ivec3>
{
    std::size_t operator()(const glm::ivec3& vec) const noexcept
    {
		return (size_t)vec.x + ((size_t)vec.y << 16ULL) + ((size_t)vec.z << 32ULL);
    }
};