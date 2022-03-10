#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace rt {

class Ray {
  public:
    Ray() {}
    Ray(const glm::vec3 &orig, const glm::vec3 &dir)
    {
        this->orig = orig;
        this->dir = dir;
    }
    glm::vec3 origin() const
    {
        return orig;
    }
    glm::vec3 direction() const
    {
        return dir;
    }
    glm::vec3 point_at_parameter(float t) const
    {
        return orig + t * dir;
    }

    glm::vec3 orig;
    glm::vec3 dir;
};

}  // namespace rt
