#pragma once
#include "../hittables/hittable.h"
#include "../math/color.h"

class material {
public:
  virtual ~material() = default;

  virtual auto scatter(const ray &r_in, const hit_record &rec,
                       color &attenuation, ray &scattered) const -> bool {
    return false;
  }

private:
  color albedo;
};

class lambertian : public material {
public:
  explicit lambertian(const color &albedo) : albedo(albedo) {}

  auto scatter(const ray &r_in, const hit_record &rec, color &attenuation,
               ray &scattered) const -> bool override;

private:
  color albedo;
};

class metal : public material {
public:
  metal(const color &albedo, double fuzz)
      : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

  auto scatter(const ray &r_in, const hit_record &rec, color &attenuation,
               ray &scattered) const -> bool override;

private:
  color albedo;
  double fuzz;
};

class dielectric : public material {
public:
  explicit dielectric(double refraction_index)
      : refraction_index(refraction_index) {}

  auto scatter(const ray &r_in, const hit_record &rec, color &attenuation,
               ray &scattered) const -> bool override;

private:
  // Refractive index in vacuum or air, or the ratio of the material's
  // refractive index over the refractive index of the enclosing media
  double refraction_index;
  static auto reflectance(double cosine, double refraction_index) -> double;
};
