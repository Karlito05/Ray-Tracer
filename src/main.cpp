#include "color.h"
#include "ray.h"
#include "vec3.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

const std::string FILENAME{"Render.ppm"};

constexpr int IMAGE_WIDTH{2560};
constexpr double ASPECT_RATIO{16.0 / 9.0};
constexpr int IMAGE_HEIGHT{static_cast<int>(IMAGE_WIDTH / ASPECT_RATIO)};

constexpr double VIEWPORT_HEIGHT{2.0};
constexpr double VIEWPORT_WIDTH{
    VIEWPORT_HEIGHT * (static_cast<double>(IMAGE_WIDTH) / IMAGE_HEIGHT)};
constexpr double FOCAL_LENGTH{1.0};

bool hit_sphere(const point3 &center, double radius, const ray &r) {
  vec3 oc = center - r.origin();
  auto a = dot(r.direction(), r.direction());
  auto b = dot(-2 * r.direction(), oc);
  auto c = dot(oc, oc) - radius * radius;

  return sqrt(b * b - 4 * a * c) >= 0;
}

color ray_color(const ray &r) {
  if (hit_sphere({0, 0, 5}, 3, r))
    return {1, 0, 0};

  vec3 unit_direction = unit_vector(r.direction());
  auto a = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

int main() {
  const point3 camera_center{0, 0, 0};

  const vec3 viewport_u{VIEWPORT_WIDTH, 0, 0};
  const vec3 viewport_v{0, -VIEWPORT_HEIGHT, 0};

  const auto pixel_delta_u{viewport_u / IMAGE_WIDTH};
  const auto pixel_delta_v{viewport_v / IMAGE_HEIGHT};

  const auto viewport_upper_left{camera_center - vec3{0, 0, FOCAL_LENGTH} -
                                 viewport_u / 2 - viewport_v / 2};
  const auto pixel00_loc{viewport_upper_left +
                         0.5 * (pixel_delta_u + pixel_delta_v)};

  std::ofstream file(FILENAME, std::ios::out);
  file << "P3\n" << IMAGE_WIDTH << ' ' << IMAGE_HEIGHT << "\n255\n";

  for (int j = 0; j < IMAGE_HEIGHT; j++) {
    std::clog << "\rScanlines Remaining: " << IMAGE_HEIGHT - j << std::flush;
    for (int i = 0; i < IMAGE_WIDTH; i++) {
      auto pixel_center =
          pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
      auto ray_direction = pixel_center - camera_center;
      ray r(camera_center, ray_direction);

      color pixel_color = ray_color(r);
      write_color(file, pixel_color);
    }
  }
}
