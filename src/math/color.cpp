#include "color.h"
#include "interval.h"
#include <iostream>

void write_color(std::ostream &out, const color &pixel_color) {
  auto r = pixel_color.r();
  auto g = pixel_color.g();
  auto b = pixel_color.b();

  // Translate the [0,1] component values to the byte range [0,255].
  const interval intesity(0.000, 0.999);

  int rbyte = int(256 * intesity.clamp(r));
  int gbyte = int(256 * intesity.clamp(g));
  int bbyte = int(256 * intesity.clamp(b));

  // Write out the pixel color components.
  out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}
