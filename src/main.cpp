#include "camera.h"
#include "hittables/hittable_list.h"
#include "hittables/sphere.h"
#include "hittables/triangle.h"
#include "materials/material.h"
#include "math/color.h"
#include "math/vec3.h"
#include "utils.h"
#include <iostream>
#include <memory>

auto main() -> int {

  std::cout << "Pick which scene you want to render:\n"
            << "[1] Material Demo\n"
            << "[2] Titlescreen\n"
            << "[3] Triangle demo\n";

  int choice{};
  std::cin >> choice;

  hittable_list world;

  camera cam;

  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 2560;
  cam.samples_per_pixel = 10;
  cam.max_depth = 50;

  switch (choice) {
  case 1: {
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
    world.add(std::make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.4,
                                       material_bubble));
    world.add(
        std::make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

    break;
  }
  case 2: {
    auto ground_material = std::make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
      for (int b = -11; b < 11; b++) {
        auto choose_mat = random_double();
        point3 center(a + 0.9 * random_double(), 0.2,
                      b + 0.9 * random_double());

        if ((center - point3(4, 0.2, 0)).length() > 0.9) {
          std::shared_ptr<material> sphere_material;

          if (choose_mat < 0.8) {
            // diffuse
            auto albedo = color::random() * color::random();
            sphere_material = std::make_shared<lambertian>(albedo);
            world.add(make_shared<sphere>(center, 0.2, sphere_material));
          } else if (choose_mat < 0.95) {
            // metal
            auto albedo = color::random(0.5, 1);
            auto fuzz = random_double(0, 0.5);
            sphere_material = std::make_shared<metal>(albedo, fuzz);
            world.add(make_shared<sphere>(center, 0.2, sphere_material));
          } else {
            // glass
            sphere_material = std::make_shared<dielectric>(1.5);
            world.add(make_shared<sphere>(center, 0.2, sphere_material));
          }
        }
      }
    }

    auto material1 = std::make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;

    break;
  }
  case 3: {
    auto material_trinagle = std::make_shared<lambertian>(color(0.5, 0.5, 0.5));

    world.add(std::make_shared<triangle>(
        std::array<point3, 3>{point3(0, 0, -1), point3(1, 0, -1),
                              point3(0, 1, -1)},
        material_trinagle));

    cam.vfov = 120;

    break;
  }
  default: {
    std::clog << "Wrong choice";
    return -1;
  }
  }

  std::clog << "Starting render. Please wait...\n";

  cam.render(world, 16);
}
