
#include "hittable.h"

class sphere : public hittable {
public:
  sphere(const point3 &center, double radius, shared_ptr<material> mat)
      : center(center), radius(std::fmax(0, radius)), mat(mat) {}

  bool hit(const ray &r, interval ray_t, hit_record &rec) const override;

private:
  point3 center;
  double radius;
  shared_ptr<material> mat;
};
