#pragma once

#include <utility>

#include "hittable.h"

class sphere : public hittable {
public:
  sphere(const point3 &center, double radius, std::shared_ptr<material> mat)
      : center(center), radius(std::fmax(0, radius)), mat(std::move(mat)) {}

  auto hit(const ray &r, interval ray_t, hit_record &rec) const
      -> bool override;

private:
  point3 center;
  double radius;
  std::shared_ptr<material> mat;
};
