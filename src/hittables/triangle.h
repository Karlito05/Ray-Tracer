#pragma once

#include <array>
#include <utility>

#include "hittable.h"

class triangle : public hittable {
public:
  triangle(const std::array<point3, 3> &vs, std::shared_ptr<material> mat)
      : a(vs[0]), b(vs[1]), c(vs[2]), mat(std::move(mat)) {}

  auto hit(const ray &r, interval ray_t, hit_record &rec) const
      -> bool override;

private:
  point3 a;
  point3 b;
  point3 c;
  std::shared_ptr<material> mat;
};
