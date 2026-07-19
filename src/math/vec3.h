#pragma once

#include "../utils.h"
#include <array>
#include <cmath>
#include <iostream>

class vec3 {
public:
  double x() const { return e[0]; }
  double y() const { return e[1]; }
  double z() const { return e[2]; }

  double r() const { return e[0]; }
  double g() const { return e[1]; }
  double b() const { return e[2]; }

  vec3() : e{0, 0, 0} {}
  vec3(double e1, double e2, double e3) : e{e1, e2, e3} {}

  vec3 operator-() const;
  vec3 &operator+=(const vec3 &v);
  vec3 &operator*=(double t);
  vec3 &operator/=(double t);
  double operator[](std::size_t i) const;
  double &operator[](std::size_t i);

  double length() const;
  double length_squared() const;
  bool near_zero() const;

  static vec3 random();

  static vec3 random(double min, double max);

  std::array<double, 3> e;
};

using point3 = vec3;

std::ostream &operator<<(std::ostream &out, const vec3 &v);

vec3 operator+(const vec3 &u, const vec3 &v);

vec3 operator-(const vec3 &u, const vec3 &v);

vec3 operator*(const vec3 &u, const vec3 &v);

vec3 operator*(double t, const vec3 &v);

vec3 operator*(const vec3 &v, double t);

vec3 operator/(const vec3 &v, double t);

vec3 unit_vector(const vec3 &v);

vec3 cross(const vec3 &u, const vec3 &v);

vec3 random_unit_vector();

vec3 random_on_hemisphere(const vec3 &normal);

vec3 random_in_unit_disk();

vec3 reflect(const vec3 &v, const vec3 &n);

vec3 refract(const vec3 &uv, const vec3 &n, double etai_over_etat);

double dot(const vec3 &u, const vec3 &v);
