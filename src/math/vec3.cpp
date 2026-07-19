#include "vec3.h"
#include "../utils.h"
#include <cmath>
#include <cstddef>
#include <ostream>

auto vec3::operator-() const -> vec3 { return {-e[0], -e[1], -e[2]}; }

auto vec3::operator[](std::size_t i) const -> double { return e[i]; }

auto vec3::operator[](std::size_t i) -> double & { return e[i]; }

auto vec3::operator+=(const vec3 &v) -> vec3 & {
  e[0] += v.e[0];
  e[1] += v.e[1];
  e[2] += v.e[2];
  return *this;
}

auto vec3::operator*=(double t) -> vec3 & {
  e[0] *= t;
  e[1] *= t;
  e[2] *= t;
  return *this;
}

auto vec3::operator/=(double t) -> vec3 & { return *this *= 1 / t; }

auto vec3::length() const -> double { return std::sqrt(length_squared()); }

auto vec3::length_squared() const -> double {
  return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
}

auto vec3::random() -> vec3 {
  return {random_double(), random_double(), random_double()};
}

auto vec3::random(double min, double max) -> vec3 {
  return {random_double(min, max), random_double(min, max),
              random_double(min, max)};
}

auto operator<<(std::ostream &out, const vec3 &v) -> std::ostream & {
  return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

auto operator+(const vec3 &u, const vec3 &v) -> vec3 {
  return {u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]};
}

auto operator-(const vec3 &u, const vec3 &v) -> vec3 {
  return {u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]};
}

auto operator*(const vec3 &u, const vec3 &v) -> vec3 {
  return {u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]};
}

auto operator*(double t, const vec3 &v) -> vec3 {
  return {t * v.e[0], t * v.e[1], t * v.e[2]};
}

auto operator*(const vec3 &v, double t) -> vec3 { return t * v; }

auto operator/(const vec3 &v, double t) -> vec3 { return (1 / t) * v; }

auto dot(const vec3 &u, const vec3 &v) -> double {
  return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

auto cross(const vec3 &u, const vec3 &v) -> vec3 {
  return {u.e[1] * v.e[2] - u.e[2] * v.e[1],
              u.e[2] * v.e[0] - u.e[0] * v.e[2],
              u.e[0] * v.e[1] - u.e[1] * v.e[0]};
}

auto unit_vector(const vec3 &v) -> vec3 { return v / v.length(); }

auto random_unit_vector() -> vec3 {
  while (true) {
    auto p = vec3::random(-1, 1);
    auto lensq = p.length_squared();
    if (1e-160 < lensq && lensq <= 1) {
      return p / sqrt(lensq);
}
  }
}

auto random_on_hemisphere(const vec3 &normal) -> vec3 {
  vec3 on_unit_sphere = random_unit_vector();
  if (dot(on_unit_sphere, normal) > 0.0) { // In the same hemisphere as the normal
    return on_unit_sphere;
  } else {
    return -on_unit_sphere;
}
}

auto vec3::near_zero() const -> bool {
  // Return true if the vector is close to zero in all dimensions.
  auto s = 1e-8;
  return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) &&
         (std::fabs(e[2]) < s);
}

auto reflect(const vec3 &v, const vec3 &n) -> vec3 { return v - 2 * dot(v, n) * n; }

auto refract(const vec3 &uv, const vec3 &n, double etai_over_etat) -> vec3 {
  auto cos_theta = std::fmin(dot(-uv, n), 1.0);
  vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
  vec3 r_out_parallel =
      -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
  return r_out_perp + r_out_parallel;
}

auto random_in_unit_disk() -> vec3 {
  while (true) {
    auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
    if (p.length_squared() < 1) {
      return p;
}
  }
}
