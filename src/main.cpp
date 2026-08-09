#include "camera.h"
#include "hittables/hittable_list.h"
#include "hittables/sphere.h"
#include "hittables/triangle.h"
#include "materials/material.h"
#include "math/color.h"
#include "math/vec3.h"
#include "popl.h"
#include "utils.h"
#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

auto main(int argc, char *argv[]) -> int {

  popl::OptionParser op("Allowed options");
  auto help_option = op.add<popl::Switch>("h", "help", "produce help message");
  auto threads = op.add<popl::Value<int>>(
      "t", "threads", "number of threads that should the ray tracer run on", 1);
  auto width = op.add<popl::Value<int>>("w", "width",
                                        "width of the resulting image", 800);
  auto samples_per_pixel = op.add<popl::Value<int>>("s", "samples_per_pixel",
                                                    "samples per pixel", 10);
  auto max_depth = op.add<popl::Value<int>>(
      "d", "depth", "maximal ammount of ray bounces", 50);
  auto path = op.add<popl::Value<std::filesystem::path>>(
      "p", "path", "a path to a STL file to render");
  auto offset = op.add<popl::Value<int>>(
      "o", "offset",
      "set an offset for the camera distance from a custom object (only active "
      "when using the path option)",
      1);
  auto fov = op.add<popl::Value<int>>(
      "f", "fov",
      "set the fov of the camera (only active when using the path option)", 90);

  op.parse(argc, argv);

  if (help_option->is_set()) {
    std::cout << op << "\n";
    return 0;
  }

  hittable_list world;

  camera cam;

  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = width->value();
  cam.samples_per_pixel = samples_per_pixel->value();
  cam.max_depth = max_depth->value();

  if (path->is_set()) {

    auto triangle_material = std::make_shared<lambertian>(color(0.4, 0.4, 0.5));

    std::ifstream file(path->value(), std::ios::binary);
    if (!file) {
      std::cerr << "Could not open file: " << path->value() << '\n';
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

    auto max_X = 0.0;
    auto max_Y = 0.0;
    auto max_Z = 0.0;

    for (uint32_t i = 0; i < triangleCount; ++i) {
      std::array<std::array<float, 3>, 3> points;
      std::array<float, 3> normal;
      file.read(reinterpret_cast<char *>(&normal), sizeof(float) * 3);
      file.read(reinterpret_cast<char *>(&points[0]), sizeof(float) * 3);
      file.read(reinterpret_cast<char *>(&points[1]), sizeof(float) * 3);
      file.read(reinterpret_cast<char *>(&points[2]), sizeof(float) * 3);

      for (const auto &point : points) {
        max_X = std::max(static_cast<double>(point[0]), max_X);
        max_Y = std::max(static_cast<double>(point[1]), max_Y);
        max_Z = std::max(static_cast<double>(point[2]), max_Z);
      }

      uint16_t attributeByteCount = 0;
      file.read(reinterpret_cast<char *>(&attributeByteCount),
                sizeof(uint16_t));

      world.add(std::make_shared<triangle>(
          std::array<point3, 3>{point3(points[0]), point3(points[1]),
                                point3(points[2])},
          triangle_material));
    }
    cam.vfov = fov->value();

    cam.lookfrom = vec3(max_X + offset->value(), max_Y + offset->value(),
                        max_Z + offset->value());
    cam.lookat = vec3(0, 0, 0);

    cam.render(world, threads->value());
    return 0;
  }

  std::cout << "Pick which scene you want to render:\n"
            << "[1] Material Demo\n"
            << "[2] Titlescreen\n"
            << "[3] Triangle demo\n"
            << "[4] Suzanne\n";

  int choice{};
  std::cin >> choice;

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

    auto triangle_material = std::make_shared<lambertian>(color(0.4, 0.4, 0.5));

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

  cam.render(world, threads->value());
}
