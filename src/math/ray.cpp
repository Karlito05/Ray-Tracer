#include "ray.h"
#include "vec3.h"

auto ray::at(double t) const -> point3 { return orig + t * dir; }
