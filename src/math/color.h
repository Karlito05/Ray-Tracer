#pragma once

#include "vec3.h"
#include <iostream>
#include <vector>

using color = vec3;

void write_colors(std::ostream &out, const std::vector<vec3> &colors, int width,
                  int height);

auto linear_to_gamma(double linear_component) -> double;
