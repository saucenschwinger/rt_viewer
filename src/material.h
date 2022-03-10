#pragma once

#include "aux.h"

namespace rt {

struct HitRecord;
class Ray;

class material {
    public:
        virtual bool scatter( const Ray& r_in, const HitRecord& rec, 
                glm::vec3& attenuation, Ray& scattered);
};

class lambertian : public material {
    public:
        glm::vec3 alfredo;

        lambertian( glm::vec3 a): alfredo(a) {
        }

        // TODO use r_in
        bool scatter( const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) {
            scattered.orig = rec.p;
            scattered.dir = rec.normal + random_in_unit_sphere();
            attenuation = alfredo;
            return true;
        }
};

}   // namespace
