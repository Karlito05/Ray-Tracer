cmake_minimum_required(VERSION 3.28)
project(RayTracing LANGUAGES CXX)

# ---- Language standard ----
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---- Options ----
option(ENABLE_CPP20_MODULE "Use the vulkan_hpp C++20 module instead of the vulkan_raii.hpp header" OFF)

# ---- Dependencies ----
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

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
        src/imgui/imgui_impl_glfw.cpp
        src/imgui/imgui_impl_vulkan.cpp
        src/imgui/imgui.cpp
        src/imgui/imgui_tables.cpp
        src/imgui/imgui_widgets.cpp
        src/imgui/imgui_draw.cpp
        src/imgui/imgui_demo.cpp
)

target_compile_definitions(RayTracer
    PRIVATE
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
)

target_link_libraries(RayTracer
    PRIVATE
        Vulkan::Vulkan
        glfw
)

# ---- Optional: C++20 module build for vulkan_hpp ----
if(ENABLE_CPP20_MODULE)
  if(CMAKE_VERSION VERSION_LESS 3.28)
    message(FATAL_ERROR "ENABLE_CPP20_MODULE requires CMake >= 3.28")
  endif()
  if(NOT CMAKE_GENERATOR MATCHES "Ninja")
    message(FATAL_ERROR "ENABLE_CPP20_MODULE requires the Ninja generator")
  endif()

  target_compile_definitions(RayTracer PRIVATE USE_CPP20_MODULES)

  # Path to vulkan_hpp module source, shipped with recent Vulkan SDKs.
  # Adjust this if your SDK places it elsewhere.
  set(VULKAN_HPP_MODULE_SRC "$ENV{VULKAN_SDK}/include/vulkan/vulkan.cppm")

  if(NOT EXISTS "${VULKAN_HPP_MODULE_SRC}")
    message(FATAL_ERROR "vulkan_hpp module source not found at ${VULKAN_HPP_MODULE_SRC}. Check your Vulkan SDK version/path.")
  endif()

  add_library(VulkanHppModule)
  target_sources(VulkanHppModule
        PUBLIC
            FILE_SET CXX_MODULES FILES
                "${VULKAN_HPP_MODULE_SRC}"
    )
  target_compile_definitions(VulkanHppModule PUBLIC VULKAN_HPP_NO_STRUCT_CONSTRUCTORS)
  target_link_libraries(VulkanHppModule PUBLIC Vulkan::Vulkan)
  set_target_properties(VulkanHppModule PROPERTIES CXX_STANDARD 23)

  target_link_libraries(RayTracer PRIVATE VulkanHppModule)
endif()
