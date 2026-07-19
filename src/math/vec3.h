#pragma once

#include <array>
#include <iostream>

class vec3 {
public:
  [[nodiscard]] auto x() const -> double { return e[0]; }
  [[nodiscard]] auto y() const -> double { return e[1]; }
  [[nodiscard]] auto z() const -> double { return e[2]; }

  [[nodiscard]] auto r() const -> double { return e[0]; }
  [[nodiscard]] auto g() const -> double { return e[1]; }
  [[nodiscard]] auto b() const -> double { return e[2]; }

  vec3() : e{0, 0, 0} {}
  vec3(double e1, double e2, double e3) : e{e1, e2, e3} {}

  auto operator-() const -> vec3;
  auto operator+=(const vec3 &v) -> vec3 &;
  auto operator*=(double t) -> vec3 &;
  auto operator/=(double t) -> vec3 &;
  auto operator[](std::size_t i) const -> double;
  auto operator[](std::size_t i) -> double &;

  [[nodiscard]] auto length() const -> double;
  [[nodiscard]] auto length_squared() const -> double;
  [[nodiscard]] auto near_zero() const -> bool;

  static auto random() -> vec3;

  static auto random(double min, double max) -> vec3;

  std::array<double, 3> e;
};

using point3 = vec3;

auto operator<<(std::ostream &out, const vec3 &v) -> std::ostream &;

auto operator+(const vec3 &u, const vec3 &v) -> vec3;

auto operator-(const vec3 &u, const vec3 &v) -> vec3;

auto operator*(const vec3 &u, const vec3 &v) -> vec3;

auto operator*(double t, const vec3 &v) -> vec3;

auto operator*(const vec3 &v, double t) -> vec3;

auto operator/(const vec3 &v, double t) -> vec3;

auto unit_vector(const vec3 &v) -> vec3;

auto cross(const vec3 &u, const vec3 &v) -> vec3;

auto random_unit_vector() -> vec3;

auto random_on_hemisphere(const vec3 &normal) -> vec3;

auto random_in_unit_disk() -> vec3;

auto reflect(const vec3 &v, const vec3 &n) -> vec3;

auto refract(const vec3 &uv, const vec3 &n, double etai_over_etat) -> vec3;

auto dot(const vec3 &u, const vec3 &v) -> double;
