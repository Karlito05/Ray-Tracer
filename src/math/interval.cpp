#include "interval.h"

double interval::clamp(double x) const {
  if (x < min)
    return min;
  if (x > max)
    return max;
  return x;
}
