cmake_minimum_required(VERSION 3.30)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Dusk VERSION 0.0.0)

include(cmake/FindDependencies.cmake)

# Interface for GLFW
add_library(dusk_glfw INTERFACE)
if(EMSCRIPTEN)
  target_link_options(dusk_glfw INTERFACE "-sUSE_GLFW=3")
else()
  target_link_libraries(dusk_glfw INTERFACE glfw)
endif()
add_library(Dusk::glfw ALIAS dusk_glfw)

# Interface for Dawn
add_library(dusk_dawn INTERFACE)
if(EMSCRIPTEN)
  target_link_libraries(dusk_dawn INTERFACE emdawnwebgpu_cpp)
  target_link_options(dusk_dawn INTERFACE "-sASYNCIFY")
else()
  target_link_libraries(dusk_dawn INTERFACE dawn::webgpu_dawn)
endif()
add_library(Dusk::dawn ALIAS dusk_dawn)

set(DUSK_PROPERTIES
    "CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_SCAN_FOR_MODULES ON
    CXX_MODULE_STD ON
    COMPILE_OPTIONS \"-Wall;-Wextra;-Wshadow;-Wpedantic;-Wsign-conversion;-Wconversion\"")

add_library(dusk_windowing)
add_library(Dusk::Windowing ALIAS dusk_windowing)
set_target_properties(dusk_windowing PROPERTIES ${DUSK_PROPERTIES})
target_sources(dusk_windowing PUBLIC
  FILE_SET CXX_MODULES
  BASE_DIRS ${CMAKE_CURRENT_LIST_DIR}
  FILES
    src/windowing/windowing.cpp
    src/windowing/System.cpp
    src/windowing/Window.cpp
    src/windowing/WindowBuilder.cpp
    src/windowing/events.cpp
    src/windowing/Event.cpp
    src/windowing/callbacks.cpp
  )

target_link_libraries(dusk_windowing PUBLIC Dusk::glfw)
target_link_options(dusk_windowing PRIVATE -stdlib=libc++)

add_library(dusk_GPU)
add_library(Dusk::GPU ALIAS dusk_GPU)
set_target_properties(dusk_GPU PROPERTIES ${DUSK_PROPERTIES})
target_sources(dusk_GPU PUBLIC
  FILE_SET CXX_MODULES
  BASE_DIRS ${CMAKE_CURRENT_LIST_DIR}
  FILES
    src/GPU/GPU.cpp
    src/GPU/IAD.cpp
    src/GPU/surface.cpp
    src/GPU/RenderPipelineBuilder.cpp)
target_link_libraries(dusk_GPU PUBLIC Dusk::dawn Dusk::glfw)
target_link_options(dusk_GPU PRIVATE -stdlib=libc++)

add_library(dusk INTERFACE)
add_library(Dusk::Dusk ALIAS dusk)
target_link_libraries(dusk INTERFACE Dusk::Windowing Dusk::GPU)
