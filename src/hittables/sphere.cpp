#include "sphere.h"
#include "../hittables/hittable.h"
#include "../math/interval.h"
#include "../math/ray.h"
#include "../math/vec3.h"
#include <cmath>

auto sphere::hit(const ray &r, interval ray_t, hit_record &rec) const -> bool {
  vec3 oc = center - r.origin();
  auto a = r.direction().length_squared();
  auto h = dot(r.direction(), oc);
  auto c = oc.length_squared() - radius * radius;

  auto discriminant = h * h - a * c;
  if (discriminant < 0) {
    return false;
}

  auto sqrtd = std::sqrt(discriminant);

  // Find the nearest root that lies in the acceptable range.
  auto root = (h - sqrtd) / a;
  if (!ray_t.surrounds(root)) {
    root = (h + sqrtd) / a;
    if (!ray_t.surrounds(root)) {
      return false;
}
  }

  rec.t = root;
  rec.p = r.at(rec.t);
  vec3 outward_normal = (rec.p - center) / radius;
  rec.set_face_normal(r, outward_normal);
  rec.mat = mat;

  return true;
}
