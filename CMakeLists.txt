
cmake_minimum_required(VERSION 4.0)
project(RayTracer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(RayTracer)

target_sources(RayTracer
PRIVATE
src/camera.cpp
src/main.cpp

src/math/vec3.cpp
src/math/ray.cpp
src/math/interval.cpp
src/math/color.cpp

src/materials/material.cpp

src/hittables/hittable.cpp
src/hittables/hittable_list.cpp
src/hittables/sphere.cpp
)
