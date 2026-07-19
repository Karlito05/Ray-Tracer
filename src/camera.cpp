#include "camera.h"
#include "hittables/hittable.h"
#include "materials/material.h" // NOLINT(misc-include-cleaner)
#include "math/color.h"
#include "math/ray.h"
#include "math/vec3.h"
#include "utils.h"
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

void camera::render(const hittable &world, int n_threads) {
  initialize();

  std::vector<std::thread> threads;
  std::vector<std::vector<vec3>> results(n_threads);

  auto start = std::chrono::high_resolution_clock::now();

  int rowsPerThread = image_height / n_threads;

  for (int i = 0; i < n_threads; ++i) {
    int start = rowsPerThread * i;
    int end = (i == n_threads - 1)
                  ? image_height // last thread eats the remainder
                  : rowsPerThread * (i + 1);

    threads.emplace_back([&results, i, this, &world, start, end]() -> void {
      results[i] = render_thread(world, start, end);
    });
  }

  for (auto &t : threads) {
    t.join(); // wait for each thread to finish
  }

  std::vector<vec3> colors{};

  for (const auto &result : results) {
    colors.append_range(result);
  }

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;

  // Write to a file
  std::ofstream file("Render.ppm", std::ios::out);

  write_colors(file, colors, image_width, image_height);
  std::clog << "Time needed: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed)
            << '\n';
}

auto camera::render_thread(const hittable &world, int start, int end) const
    -> std::vector<vec3> {

  if (!is_initialized) {
    throw "Camera isn't initialized!";
  }

  std::vector<vec3> result;

  // std::ofstream file("Render.ppm", std::ios::out);

  // file << "P3\n" << image_width << ' ' << image_height << "\n255\n";

  for (int j = start; j < end; j++) {

    for (int i = 0; i < image_width; i++) {
      // std::clog << "\rScanlines remaining: " << (end - j)
      //           << " Pixels till next scanline: " << (image_width - i) << ' '
      //           << std::flush;

      color pixel_color(0, 0, 0);
      for (int sample = 0; sample < samples_per_pixel; sample++) {
        ray r = get_ray(i, j);
        pixel_color += ray_color(r, max_depth, world);
      }
      result.push_back(pixel_samples_scale * pixel_color);
    }
  }

  std::clog << "Done.\n";
  return result;
}

void camera::initialize() {
  image_height = static_cast<int>(image_width / aspect_ratio);
  image_height = (image_height < 1) ? 1 : image_height;

  pixel_samples_scale = 1.0 / samples_per_pixel;

  center = lookfrom;

  // Determine viewport dimensions.
  auto theta = degrees_to_radians(vfov);
  auto h = std::tan(theta / 2);
  auto viewport_height = 2 * h * focus_dist;
  auto viewport_width =
      viewport_height * (static_cast<double>(image_width) / image_height);

  // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
  w = unit_vector(lookfrom - lookat);
  u = unit_vector(cross(vup, w));
  v = cross(w, u);

  // Calculate the vectors across the horizontal and down the vertical viewport
  // edges.
  vec3 viewport_u =
      viewport_width * u; // Vector across viewport horizontal edge
  vec3 viewport_v = viewport_height * -v; // Vector down viewport vertical edge

  // Calculate the horizontal and vertical delta vectors from pixel to pixel.
  pixel_delta_u = viewport_u / image_width;
  pixel_delta_v = viewport_v / image_height;

  // Calculate the location of the upper left pixel.
  auto viewport_upper_left =
      center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
  pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

  // Calculate the camera defocus disk basis vectors.
  auto defocus_radius =
      focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
  defocus_disk_u = u * defocus_radius;
  defocus_disk_v = v * defocus_radius;

  is_initialized = true;
}

auto camera::ray_color(const ray &r, int depth, const hittable &world) const
    -> color {

  if (depth <= 0) {
    return {0, 0, 0};
  }

  hit_record rec;

  if (world.hit(r, interval(0.001, infinity), rec)) {
    ray scattered;
    color attenuation;
    if (rec.mat->scatter(r, rec, attenuation, scattered)) {
      return attenuation * ray_color(scattered, depth - 1, world);
    }
    return {0, 0, 0};
  }

  vec3 unit_direction = unit_vector(r.direction());
  auto a = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

auto camera::get_ray(int i, int j) const -> ray {
  // Construct a camera ray originating from the origin and directed at randomly
  // sampled point around the pixel location i, j.

  auto offset = sample_square();
  auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) +
                      ((j + offset.y()) * pixel_delta_v);

  auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
  auto ray_direction = pixel_sample - ray_origin;

  return {ray_origin, ray_direction};
}

auto camera::defocus_disk_sample() const -> point3 {
  // Returns a random point in the camera defocus disk.
  auto p = random_in_unit_disk();
  return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
}

auto camera::sample_square() const -> vec3 {
  // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit
  // square.
  return {random_double() - 0.5, random_double() - 0.5, 0};
}
