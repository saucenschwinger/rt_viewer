#pragma once

#include "aux.h"

namespace rt {

class material {
    public:
        virtual bool scatter(
                const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) = 0;
};

class lambertian : public material {
    public:
        glm::vec3 alfredo;

        lambertian( glm::vec3 a): alfredo(a) {
        }

        bool scatter( const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) {
            attenuation = alfredo;
            scattered.orig = rec.p;
            scattered.dir = rec.normal + random_in_unit_sphere();
            return true;
        }
};

}   // namespace
