#include "vec3.h"

vec3 vec3::operator-() const { return vec3(-e[0], -e[1], -e[2]); }

double vec3::operator[](std::size_t i) const { return e[i]; }

double &vec3::operator[](std::size_t i) { return e[i]; }

vec3 &vec3::operator+=(const vec3 &v) {
  e[0] += v.e[0];
  e[1] += v.e[1];
  e[2] += v.e[2];
  return *this;
}

vec3 &vec3::operator*=(double t) {
  e[0] *= t;
  e[1] *= t;
  e[2] *= t;
  return *this;
}

vec3 &vec3::operator/=(double t) { return *this *= 1 / t; }

double vec3::length() const { return std::sqrt(length_squared()); }

double vec3::length_squared() const {
  return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
}

std::ostream &operator<<(std::ostream &out, const vec3 &v) {
  return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

vec3 operator+(const vec3 &u, const vec3 &v) {
  return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

vec3 operator-(const vec3 &u, const vec3 &v) {
  return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

vec3 operator*(const vec3 &u, const vec3 &v) {
  return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

vec3 operator*(double t, const vec3 &v) {
  return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

vec3 operator*(const vec3 &v, double t) { return t * v; }

vec3 operator/(const vec3 &v, double t) { return (1 / t) * v; }

double dot(const vec3 &u, const vec3 &v) {
  return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

vec3 cross(const vec3 &u, const vec3 &v) {
  return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
              u.e[2] * v.e[0] - u.e[0] * v.e[2],
              u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

vec3 unit_vector(const vec3 &v) { return v / v.length(); }
