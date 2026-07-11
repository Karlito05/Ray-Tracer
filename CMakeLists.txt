cmake_minimum_required(VERSION 4.3.4)

project(RayTracing)

add_executable(RayTracer)

target_sources(RayTracer
PRIVATE
src/main.cpp
src/camera.cpp

src/math/vec3.cpp
src/math/color.cpp
src/math/ray.cpp
src/math/interval.cpp

src/hittables/hittable.cpp
src/hittables/sphere.cpp
src/hittables/hittable_list.cpp

src/materials/material.cpp

src/imgui_impl_glfw.cpp
src/imgui_impl_vulkan.cpp
src/imgui.cpp
src/imgui_tables.cpp
src/imgui_widgets.cpp
src/imgui_draw.cpp
src/imgui_demo.cpp
)

target_link_libraries(RayTracer glfw vulkan)
