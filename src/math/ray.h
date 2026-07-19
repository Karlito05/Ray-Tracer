#pragma once
#include "vec3.h"

class ray {
public:
  ray() = default;
  ray(const point3 &origin, const vec3 &direction)
      : orig(origin), dir(direction) {}

  [[nodiscard]] auto origin() const -> const point3 & { return orig; }
  [[nodiscard]] auto direction() const -> const vec3 & { return dir; }

  [[nodiscard]] auto at(double t) const -> point3;

private:
  point3 orig;
  vec3 dir;
};
