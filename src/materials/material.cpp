#include "material.h"
bool lambertian::scatter(const ray &r_in, const hit_record &rec,
                         color &attenuation, ray &scattered) const {
  auto scatter_direction = rec.normal + random_unit_vector();

  if (scatter_direction.near_zero())
    scatter_direction = rec.normal;

  scattered = ray(rec.p, scatter_direction);
  attenuation = albedo;
  return true;
}

bool metal::scatter(const ray &r_in, const hit_record &rec, color &attenuation,
                    ray &scattered) const {
  vec3 reflected = reflect(r_in.direction(), rec.normal);
  scattered = ray(rec.p, reflected);
  attenuation = albedo;
  return true;
}
