
#include "../math/vec3.h"
#include "hittable.h"

class sphere : public hittable {
public:
  sphere(const point3 &center, double radius)
      : center(center), radius(std::fmax(0, radius)) {}

  bool hit(const ray &r, double ray_tmin, double ray_tmax,
           hit_record &rec) const override;

private:
  point3 center;
  double radius;
};
