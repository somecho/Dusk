cmake_minimum_required(VERSION 3.30)

project(Dusk VERSION 0.0.11)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/FindDependencies.cmake)

add_library(${PROJECT_NAME})
add_library(Dusk::Dusk ALIAS ${PROJECT_NAME})

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES CXX_STANDARD 23
             CXX_STANDARD_REQUIRED ON
             CXX_SCAN_FOR_MODULES ON
	     CXX_MODULE_STD ON
             COMPILE_OPTIONS
             "-Wall;-Wextra;-Wshadow;-Wpedantic;-Wsign-conversion;-Wconversion")

target_sources(${PROJECT_NAME}
  PUBLIC
  FILE_SET CXX_MODULES
  BASE_DIRS ${CMAKE_CURRENT_LIST_DIR}
  FILES
    Dusk/Window.cpp
    Dusk/Log.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC dawn::webgpu_dawn glfw glm)

target_link_options(${PROJECT_NAME} PRIVATE -stdlib=libc++)
