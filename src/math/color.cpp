#include "color.h"
#include "interval.h"
#include <iostream>

void write_color(std::ostream &out, const color &pixel_color) {
  auto r = linear_to_gamma(pixel_color.r());
  auto g = linear_to_gamma(pixel_color.g());
  auto b = linear_to_gamma(pixel_color.b());

  // Translate the [0,1] component values to the byte range [0,255].
  const interval intesity(0.000, 0.999);

  int rbyte = int(256 * intesity.clamp(r));
  int gbyte = int(256 * intesity.clamp(g));
  int bbyte = int(256 * intesity.clamp(b));

  // Write out the pixel color components.
  out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

double linear_to_gamma(double linear_component) {
  if (linear_component > 0)
    return std::sqrt(linear_component);

  return 0;
}
