#include "interval.h"

auto interval::clamp(double x) const -> double {
  if (x < min) {
    return min;
}
  if (x > max) {
    return max;
}
  return x;
}
