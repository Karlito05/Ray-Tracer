#include "triangle.h"
#include "../hittables/hittable.h"
#include "../math/interval.h"
#include "../math/ray.h"
#include "../math/vec3.h"
#include <cmath>

auto triangle::hit(const ray &r, interval ray_t, hit_record &rec) const
    -> bool {
  constexpr float epsilon = std::numeric_limits<double>::epsilon();

  vec3 edge1 = b - a;
  vec3 edge2 = c - a;

  // Backface culling, assuming CCW-wound triangles.
  const vec3 normal = cross(edge1, edge2); // No need to normalize
  if (dot(normal, r.direction()) > 0) {
    return false;
  }

  vec3 ray_cross_e2 = cross(r.direction(), edge2);
  double det = dot(edge1, ray_cross_e2);

  if (std::abs(det) < epsilon) {
    return false; // Ray is parallel to triangle
  }

  double inv_det = 1.0 / det;
  vec3 s = r.origin() - a;
  double u = inv_det * dot(s, ray_cross_e2);

  if (u < -epsilon || u - 1 > epsilon) {
    return false; // Ray passes outside edge2's bounds
  }

  vec3 s_cross_e1 = cross(s, edge1);
  double v = inv_det * dot(r.direction(), s_cross_e1);

  if (v < -epsilon || u + v - 1 > epsilon) {
    return false; // Ray passes outside edge1's bounds
  }

  // The ray line intersects with the triangle.
  // We compute t to find where on the ray the intersection is.
  double t = inv_det * dot(edge2, s_cross_e1);

  if (t > epsilon) // Ray intersection
  {
    rec.t = t;
    rec.p = r.at(rec.t);

    rec.set_face_normal(r, normal);
    rec.mat = mat;

    return true;
  } else { // This means that there is a line intersection but not a ray
           // intersection.
    return false;
  }
}
