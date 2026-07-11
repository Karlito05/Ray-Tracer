#pragma once
#include "../hittables/hittable.h"
#include "../math/color.h"

class material {
public:
  virtual ~material() = default;

  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       color &attenuation, ray &scattered) const {
    return false;
  }

private:
  color albedo;
};

class lambertian : public material {
public:
  lambertian(const color &albedo) : albedo(albedo) {}

  bool scatter(const ray &r_in, const hit_record &rec, color &attenuation,
               ray &scattered) const override;

private:
  color albedo;
};

class metal : public material {
public:
  metal(const color &albedo) : albedo(albedo) {}

  bool scatter(const ray &r_in, const hit_record &rec, color &attenuation,
               ray &scattered) const override;

private:
  color albedo;
};
