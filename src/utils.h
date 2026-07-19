#pragma once

#include <limits>
#include <numbers>
#include <random>

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = std::numbers::pi;

// Utility Functions

inline auto degrees_to_radians(double degrees) -> double {
  return degrees * pi / 180.0;
}

inline auto random_double() -> double {
  static std::uniform_real_distribution<double> distribution(0.0, 1.0);
  static std::mt19937 generator{std::random_device{}()};
  return distribution(generator);
}

inline auto random_double(double min, double max) -> double {
  // Returns a random real in [min,max).
  return min + ((max - min) * random_double());
}
