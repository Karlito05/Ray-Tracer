#include "camera.h"
#include "hittables/hittable_list.h"
#include "hittables/sphere.h"
#include "materials/material.h"
#include "math/color.h"
#include "math/vec3.h"
#include <memory>

auto main() -> int {

  hittable_list world;

  auto material_ground = std::make_shared<lambertian>(color(0.8, 0.8, 0.0));
  auto material_center = std::make_shared<lambertian>(color(0.1, 0.2, 0.5));
  auto material_left = std::make_shared<dielectric>(1.50);
  auto material_bubble = std::make_shared<dielectric>(1.00 / 1.50);
  auto material_right = std::make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

  world.add(std::make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0,
                                     material_ground));
  world.add(
      std::make_shared<sphere>(point3(0.0, 0.0, -1.2), 0.5, material_center));
  world.add(
      std::make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
  world.add(
      std::make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.4, material_bubble));
  world.add(
      std::make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

  camera cam;

  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 2560;
  cam.samples_per_pixel = 100;
  cam.max_depth = 50;

  cam.vfov = 50;
  cam.lookfrom = point3(-2, 2, 1);
  cam.lookat = point3(0, 0, -1);
  cam.vup = vec3(0, 1, 0);

  cam.defocus_angle = 10.0;
  cam.focus_dist = 3.4;

  cam.render(world, 16);
}
