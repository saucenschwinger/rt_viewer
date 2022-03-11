#pragma once

#include <cstdlib>
#include <math.h>

//TODO proper citation
/*
 * From raytracing in a week-end book
 */
inline double random_double() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();
}
/* copy end
 */

inline glm::vec3 random_vec3(double min, double max) {
    return glm::vec3(random_double(min, max), random_double(min, max), random_double(min, max));
}

double length_of_vec3(glm::vec3 v) {
    return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

glm::vec3 random_in_unit_sphere() {
    while (true) {
        auto p = random_vec3(-1,1);
        double len_p = length_of_vec3(p);
        if (len_p*len_p >= 1) continue;
        return p;
    }
}

// copied from "Raytracing in One Weekend"
glm::vec3 random_in_hemisphere(const glm::vec3& normal) {
    glm::vec3 in_unit_sphere = random_in_unit_sphere();
    if (glm::dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

inline bool near_zero(glm::vec3 v) {
    return fabs(v[0]) < 1e-3 && fabs(v[1]) < 1e-3 && fabs(v[2]) < 1e-3;
}


