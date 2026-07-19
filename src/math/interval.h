#pragma once
#include "../utils.h"

class interval {
public:
  double min, max;

  interval() : min(+infinity), max(-infinity) {} // Default interval is empty

  interval(double min, double max) : min(min), max(max) {}

  [[nodiscard]] auto size() const -> double { return max - min; }

  [[nodiscard]] auto contains(double x) const -> bool { return min <= x && x <= max; }

  [[nodiscard]] auto surrounds(double x) const -> bool { return min < x && x < max; }

  [[nodiscard]] auto clamp(double x) const -> double;

  static const interval empty, universe;
};

// const interval interval::empty = interval(+infinity, -infinity);
// const interval interval::universe = interval(-infinity, +infinity);
