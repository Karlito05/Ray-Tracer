#include "color.h"
#include "vec3.h"
#include <fstream>
#include <iostream>
#include <string>

constexpr int IMAGE_WIDTH{256};
constexpr int IMAGE_HEIGHT{256};
const std::string FILENAME{"Render.ppm"};
int main() {
  std::ofstream file(FILENAME, std::ios::out);
  file << "P3\n" << IMAGE_WIDTH << ' ' << IMAGE_HEIGHT << "\n255\n";

  for (int j = 0; j < IMAGE_HEIGHT; j++) {
    std::clog << "\rScanlines Remaining: " << IMAGE_HEIGHT - j << std::flush;
    for (int i = 0; i < IMAGE_WIDTH; i++) {
      auto pixelColor = color(static_cast<double>(i) / (IMAGE_WIDTH - 1),
                              static_cast<double>(j) / (IMAGE_HEIGHT - 1), 0);
      write_color(file, pixelColor);
    }
  }
}
