#include "camera.h"
#include "hittables/hittable_list.h"
#include "hittables/sphere.h"
#include "hittables/triangle.h"
#include "materials/material.h"
#include "math/color.h"
#include "math/vec3.h"
#include "utils.h"
#include <array>
#include <fstream>
#include <iostream>
#include <memory>

auto main() -> int {

  std::cout << "Pick which scene you want to render:\n"
            << "[1] Material Demo\n"
            << "[2] Titlescreen\n"
            << "[3] Triangle demo\n"
            << "[4] Suzanne\n";

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
  case 4: {
    auto ground_material = std::make_shared<lambertian>(color(0.2, 0.2, 0.2));
    world.add(make_shared<sphere>(point3(0, -1001, 0), 1000, ground_material));

    auto triangle_material = std::make_shared<metal>(color(0.4, 0.4, 0.5), 0);

    std::ifstream file("Suzanne.stl", std::ios::binary);
    if (!file) {
      std::cerr << "Could not open file: Suzanne.stl";
      return 1;
    }

    // Skip 80-byte header
    file.seekg(80, std::ios::beg);

    uint32_t triangleCount = 0;
    file.read(reinterpret_cast<char *>(&triangleCount), sizeof(uint32_t));

    if (!file) {
      std::cerr << "File too small to contain a valid STL header";
      return 1;
    }

    for (uint32_t i = 0; i < triangleCount; ++i) {
      std::array<std::array<float, 3>, 3> points;
      std::array<float, 3> normal;
      file.read(reinterpret_cast<char *>(&normal), sizeof(float) * 3);
      file.read(reinterpret_cast<char *>(&points[0]), sizeof(float) * 3);
      file.read(reinterpret_cast<char *>(&points[1]), sizeof(float) * 3);
      file.read(reinterpret_cast<char *>(&points[2]), sizeof(float) * 3);

      uint16_t attributeByteCount = 0;
      file.read(reinterpret_cast<char *>(&attributeByteCount),
                sizeof(uint16_t));

      world.add(std::make_shared<triangle>(
          std::array<point3, 3>{point3(points[0]), point3(points[1]),
                                point3(points[2])},
          triangle_material));
    }
    cam.vfov = 40;

    cam.lookfrom = vec3(2.5, 3, 3);
    cam.lookat = vec3(0, 0, 0);
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
