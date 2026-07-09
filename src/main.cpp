
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
    for (int i = 0; i < IMAGE_WIDTH; i++) {
      auto r = double(i) / (IMAGE_WIDTH - 1);
      auto g = double(j) / (IMAGE_HEIGHT - 1);
      auto b = 0.0;

      int ir = int(255.999 * r);
      int ig = int(255.999 * g);
      int ib = int(255.999 * b);

      file << ir << ' ' << ig << ' ' << ib << '\n';
    }
  }
}
