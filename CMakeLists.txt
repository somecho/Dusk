cmake_minimum_required(VERSION 3.30)

project(Dusk VERSION 0.0.11)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/FindDependencies.cmake)

add_library(${PROJECT_NAME}_windowing)
add_library(Dusk::Windowing ALIAS ${PROJECT_NAME}_windowing)

set_target_properties(
  ${PROJECT_NAME}_windowing
  PROPERTIES CXX_STANDARD 23
             CXX_STANDARD_REQUIRED ON
             CXX_SCAN_FOR_MODULES ON
	     CXX_MODULE_STD ON
             COMPILE_OPTIONS
             "-Wall;-Wextra;-Wshadow;-Wpedantic;-Wsign-conversion;-Wconversion")

target_sources(${PROJECT_NAME}_windowing
  PUBLIC
  FILE_SET CXX_MODULES
  BASE_DIRS ${CMAKE_CURRENT_LIST_DIR}
  FILES
    src/windowing/windowing.cpp
    src/windowing/System.cpp
    src/windowing/Window.cpp
    src/windowing/WindowBuilder.cpp
)

target_link_libraries(${PROJECT_NAME}_windowing PUBLIC dawn::webgpu_dawn glfw)

target_link_options(${PROJECT_NAME}_windowing PRIVATE -stdlib=libc++)

add_library(${PROJECT_NAME}_GPU)
add_library(Dusk::GPU ALIAS ${PROJECT_NAME}_GPU)
set_target_properties(
  ${PROJECT_NAME}_GPU
  PROPERTIES CXX_STANDARD 23
             CXX_STANDARD_REQUIRED ON
             CXX_SCAN_FOR_MODULES ON
	     CXX_MODULE_STD ON
             COMPILE_OPTIONS
             "-Wall;-Wextra;-Wshadow;-Wpedantic;-Wsign-conversion;-Wconversion")

target_sources(${PROJECT_NAME}_GPU
  PUBLIC
  FILE_SET CXX_MODULES
  BASE_DIRS ${CMAKE_CURRENT_LIST_DIR}
  FILES
    src/GPU/GPU.cpp
    src/GPU/glfw.cpp
)
target_link_libraries(${PROJECT_NAME}_GPU PUBLIC dawn::webgpu_dawn glfw)
target_link_options(${PROJECT_NAME}_GPU PRIVATE -stdlib=libc++)


add_library(${PROJECT_NAME} INTERFACE)
add_library(Dusk::Dusk ALIAS ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} INTERFACE Dusk::Windowing Dusk::GPU)
