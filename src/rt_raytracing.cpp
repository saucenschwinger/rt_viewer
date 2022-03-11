#include "rt_raytracing.h"
#include "rt_ray.h"
#include "rt_hitable.h"
#include "rt_sphere.h"
#include "rt_triangle.h"
#include "rt_box.h"
#include "material.h"
#include "aux.h"

#include "cg_utils2.h"  // Used for OBJ-mesh loading
#include <stdlib.h>     // Needed for drand48()
#include <memory>

namespace rt {

struct HitRecord;

// Store scene (world) in a global variable for convenience
struct Scene {
    Sphere ground;
    std::vector<Sphere> spheres;
    std::vector<Box> boxes;
    std::vector<Triangle> mesh;
    Box mesh_bbox;
} g_scene;

bool hit_world(const Ray &r, float t_min, float t_max, HitRecord &rec)
{
    HitRecord temp_rec;
    bool hit_anything = false;
    float closest_so_far = t_max;

    if (g_scene.ground.hit(r, t_min, closest_so_far, temp_rec)) {
        hit_anything = true;
        closest_so_far = temp_rec.t;
        rec = temp_rec;
    }
    for (int i = 0; i < g_scene.spheres.size(); ++i) {
        if (g_scene.spheres[i].hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    for (int i = 0; i < g_scene.boxes.size(); ++i) {
        if (g_scene.boxes[i].hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    if (g_scene.mesh_bbox.hit(r, t_min, closest_so_far, temp_rec)) {
        for (int i = 0; i < g_scene.mesh.size(); ++i) {
            if (g_scene.mesh[i].hit(r, t_min, closest_so_far, temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }
    }

    return hit_anything;
}

// This function should be called recursively (inside the function) for
// bouncing rays when you compute the lighting for materials, like this
//
// if (hit_world(...)) {
//     ...
//     return color(rtx, r_bounce, max_bounces - 1);
// }
//
// See Chapter 7 in the "Ray Tracing in a Weekend" book
glm::vec3 color(RTContext &rtx, const Ray &r, int max_bounces)
{
    if (max_bounces < 0) return glm::vec3(0.0f);

    HitRecord rec;
    if (hit_world(r, 0.001f, 1000., rec)) {
        rec.normal = glm::normalize(rec.normal);  // Always normalise before use!
        if (rtx.show_normals) { return rec.normal * 0.5f + 0.5f; }

        Ray target;
        glm::vec3 att = glm::vec3(1.);
        // TODO check scatter ret val
        if (rec.mat->scatter(r, rec, att, target)) {
            return att*color(rtx, target, max_bounces--);
        } else {
            return glm::vec3(0.);
        }
    }

    // If no hit, return sky color
    glm::vec3 unit_direction = glm::normalize(r.direction());
    float t = 0.5f * (unit_direction.y + 1.0f);
    return (1.0f - t) * rtx.ground_color + t * rtx.sky_color;
}

// MODIFY THIS FUNCTION!
void setupScene(RTContext &rtx, const char *filename)
{
    auto red    = std::make_shared<lambertian>(glm::vec3(1.,0.1,0.1));
    auto ggreen = std::make_shared<lambertian>(glm::vec3(0.01,154/255.,23/255.));
    auto geld   = std::make_shared<lambertian>(glm::vec3(1.,215/255.,0.01));
    auto ibm    = std::make_shared<lambertian>(glm::vec3(0.01, 98/255., 1.));
    auto chrome = std::make_shared<metallic>(glm::vec3(219/255.,226/255.,233/255.));

    g_scene.ground = Sphere(glm::vec3(0.0f, -1000.5f, 0.0f), 1000.0f,  ggreen);
#if 1
    g_scene.spheres = {
        Sphere(glm::vec3(0.0f, -0.3f, 1.0f), 0.2f, geld),
        Sphere(glm::vec3(1.5f, -0.3f, 0.0f) , 0.9f, red ),
        Sphere(glm::vec3(-1.3f, -0.3f, 0.0f), 0.3f, ibm ),
    };
#endif

    glm::vec3 transl = glm::vec3(-0.3f, 0.15, -1.0f);

#if 1
    cg::OBJMesh mesh;
    cg::objMeshLoad(mesh, filename);
    g_scene.mesh.clear();
    mesh.mat = chrome;
    glm::vec3 ub;   // upper bound for x,y,z
    glm::vec3 lb;   // lower bound for x,y,z
    for (int i = 0; i < mesh.indices.size(); i += 3) {
        int i0 = mesh.indices[i + 0];
        int i1 = mesh.indices[i + 1];
        int i2 = mesh.indices[i + 2];
        glm::vec3 v0 = mesh.vertices[i0] + transl;
        glm::vec3 v1 = mesh.vertices[i1] + transl;
        glm::vec3 v2 = mesh.vertices[i2] + transl;
        ub = glm::max(glm::max(glm::max(ub, v0), v1), v2);
        lb = glm::min(glm::min(glm::min(lb, v0), v1), v2);
        g_scene.mesh.push_back(Triangle(v0, v1, v2, mesh.mat));
    }
    glm::vec3 cent = 0.5f*(ub+lb);
    glm::vec3 rad = glm::abs(ub-lb)*0.5f;
    
    //ub = glm::max(ub, -1.f*lb);
    printf("ub:%.2f %.2f %.2f\n", ub[0], ub[1], ub[2]);
    printf("lb:%.2f %.2f %.2f\n", lb[0], lb[1], lb[2]);
    printf("bbox: cent:(%.2f %.2f %.2f), rad:(%.2f %.2f %.2f)\n",  cent[0],cent[1],cent[2],rad[0],rad[1],rad[2]);

    auto transp = std::make_shared<transparent>(glm::vec3(0.8, 0.8, 1.), 0.7);
    g_scene.mesh_bbox = Box(cent, rad, transp);
#endif


#if 1
    g_scene.boxes = {
        g_scene.mesh_bbox = Box(cent, rad, transp),
        Box(glm::vec3(-0.5f, -0.1, -1.0f), glm::vec3(0.1), chrome),
        Box(glm::vec3(-0.6f, -0.5, 1.0f), glm::vec3(0.08f), red),
        Box(glm::vec3(0.9f, -0.5f, 1.0f), glm::vec3(0.05), ibm),
        Box(glm::vec3(0.5f, -0.5f, -4.f), glm::vec3(1.5), geld)
    };
#endif
}

// MODIFY THIS FUNCTION!
void updateLine(RTContext &rtx, int y)
{
    int nx = rtx.width;
    int ny = rtx.height;
    float aspect = float(nx) / float(ny);
    glm::vec3 lower_left_corner(-1.0f * aspect, -1.0f, -1.0f);
    glm::vec3 horizontal(2.0f * aspect, 0.0f, 0.0f);
    glm::vec3 vertical(0.0f, 2.0f, 0.0f);
    glm::vec3 origin(0.0f, 0.0f, 0.0f);
    glm::mat4 world_from_view = glm::inverse(rtx.view);

    // You can try parallelising this loop by uncommenting this line:
    int samples = rtx.samples;
    #pragma omp parallel for schedule(guided) num_threads(4)
    for (int x = 0; x < nx; ++x) {
        glm::vec3 c;
        for (int s=0; s < samples; s++) {
            float u = (float(x) + random_double(0, 1)) / float(nx);
            float v = (float(y) + random_double(0, 1)) / float(ny);
            Ray r(origin, lower_left_corner + u * horizontal + v * vertical);
            r.orig = glm::vec3(world_from_view * glm::vec4(r.orig, 1.0f));
            r.dir = glm::vec3(world_from_view * glm::vec4(r.dir, 0.0f));

            // Note: in the RTOW book, they have an inner loop for the number of
            // samples per pixel. Here, you do not need this loop, because we want
            // some interactivity and accumulate samples over multiple frames
            // instead (until the camera moves or the rendering is reset).

            
            c += color(rtx, r, rtx.max_bounces);
        }

        if (rtx.smooth_update) {
            if (rtx.current_frame <= 0) {
                    // Here we make the first frame blend with the old image,
                    // to smoothen the transition when resetting the accumulation
                    glm::vec4 old = rtx.image[y * nx + x];
                    rtx.image[y * nx + x] = glm::clamp(old / glm::max(1.0f, old.a), 0.0f, 1.0f);
            }
            rtx.image[y * nx + x] += glm::vec4(c/glm::vec3(samples), 1.0f);
        } else {
            rtx.image[y * nx + x] = glm::vec4(c/glm::vec3(samples), 1.0f);
        }
    }
}

void updateImage(RTContext &rtx)
{
    if (rtx.freeze) return;                    // Skip update
    rtx.image.resize(rtx.width * rtx.height);  // Just in case...

    updateLine(rtx, rtx.current_line % rtx.height);

    if (rtx.current_frame < rtx.max_frames) {
        rtx.current_line += 1;
        if (rtx.current_line >= rtx.height) {
            rtx.current_frame += 1;
            rtx.current_line = rtx.current_line % rtx.height;
        }
    }
}

void resetImage(RTContext &rtx)
{
    rtx.image.clear();
    rtx.image.resize(rtx.width * rtx.height);
    rtx.current_frame = 0;
    rtx.current_line = 0;
    rtx.freeze = false;
}

void resetAccumulation(RTContext &rtx)
{
    rtx.current_frame = -1;
}

}  // namespace rt
