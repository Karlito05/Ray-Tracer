cmake_minimum_required(VERSION 4.3.4)

project(RayTracing)

add_executable(RayTracer)

target_sources(RayTracer
PRIVATE
src/main.cpp
src/math/vec3.cpp
src/math/color.cpp
src/math/ray.cpp
)
