#pragma once

#include "aux.h"

namespace rt {

struct HitRecord;
class Ray;

class material {
    public:
        virtual bool scatter( const Ray& r_in, const HitRecord& rec, 
                glm::vec3& attenuation, Ray& scattered) = 0;
};

class lambertian : public material {
    public:
        glm::vec3 alfredo;

        lambertian( glm::vec3 a): alfredo(a) {
        }

        bool scatter( const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) {
            // TODO
            //  // Catch degenerate scatter direction
            scattered.orig = rec.p;
            scattered.dir = rec.normal + random_in_hemisphere(rec.normal);

            if (near_zero(scattered.dir))
                scattered.dir = rec.normal;
            attenuation = alfredo;
            return (glm::dot(scattered.dir, rec.normal) > 0);
        }
};

class metallic : public material {
    public:
        glm::vec3 alfredo;

        metallic( glm::vec3 a): alfredo(a) {}

        static glm::vec3 reflect(const glm::vec3& v, const glm::vec3& n) {
            return v - 2*glm::dot(v,n)*n;
        }

        bool scatter( const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) {
            scattered.orig = rec.p;
            scattered.dir = reflect(glm::normalize(r_in.dir), rec.normal);
            if (near_zero(scattered.dir))
                scattered.dir = rec.normal;
            attenuation = alfredo;
            return (glm::dot(scattered.dir, rec.normal) > 0);
        }
};

class transparent : public material {
    public:
        glm::vec3 alfredo;
        float alpha;

        transparent( glm::vec3 a, double alpha): alfredo(a), alpha(alpha) {}

        static glm::vec3 reflect(const glm::vec3& v, const glm::vec3& n) {
            return v - 2*glm::dot(v,n)*n;
        }

        bool scatter( const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) {
            // show some reflections, doesnt really work
            if (random_double() > alpha) {
                //attenuation = (1-alpha)*alfredo + alpha*glm::vec3(1.);
                attenuation = alfredo;
                // reflect
                scattered.orig = rec.p;
                scattered.dir = reflect(glm::normalize(r_in.dir), rec.normal);
                if (near_zero(scattered.dir))
                    scattered.dir = rec.normal;
                return true;
            } else {
                // pass through
                attenuation = (1-alpha)*alfredo + alpha*glm::vec3(1.);
                scattered.dir = r_in.dir;
                scattered.orig = rec.p;
                if (near_zero(scattered.dir))
                    scattered.dir = rec.normal;
                return true;
            }
        }
};

}   // namespace
