cmake_minimum_required(VERSION 4.0)
project(RayTracer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default to Release if the user didn't specify a build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

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

# ---- Performance flags (Release only) ----
target_compile_options(RayTracer PRIVATE
  $<$<CONFIG:Release>:-O3>
  $<$<CONFIG:Release>:-march=native>
  $<$<CONFIG:Release>:-mtune=native>
  $<$<CONFIG:Release>:-funroll-loops>
  $<$<CONFIG:Release>:-ffast-math>
  $<$<CONFIG:Release>:-DNDEBUG>
)

# ---- Link-Time Optimization ----
include(CheckIPOSupported)
check_ipo_supported(RESULT LTO_SUPPORTED OUTPUT LTO_ERROR)
if(LTO_SUPPORTED)
  set_target_properties(RayTracer PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE ON
  )
else()
  message(WARNING "LTO not supported: ${LTO_ERROR}")
endif()

# ---- Threading ----
find_package(Threads REQUIRED)
target_link_libraries(RayTracer PRIVATE Threads::Threads)
