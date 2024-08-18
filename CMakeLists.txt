cmake_minimum_required(VERSION 3.15)
project(Dusk VERSION 0.0.1)

add_library(${PROJECT_NAME})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

set(${PROJECT_NAME}_INCLUDES
    ${PROJECT_NAME}/App.hpp
)

target_sources(${PROJECT_NAME}
    PRIVATE
        ${${PROJECT_NAME}_INCLUDES}
        ${PROJECT_NAME}/App.cpp
)

find_package(Dawn REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC dawn::webgpu_dawn glfw)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

