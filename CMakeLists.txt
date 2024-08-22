cmake_minimum_required(VERSION 3.15)
project(Dusk VERSION 0.0.9)

add_library(${PROJECT_NAME})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_compile_definitions(${PROJECT_NAME} PRIVATE RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/resources")

set(${PROJECT_NAME}_INCLUDES
    ${PROJECT_NAME}/App.hpp
    ${PROJECT_NAME}/Drawer.hpp
    ${PROJECT_NAME}/Shader.hpp
    ${PROJECT_NAME}/Builder/Buffer.hpp
    ${PROJECT_NAME}/Interface.hpp
    ${PROJECT_NAME}/Drawables.hpp
)

target_sources(${PROJECT_NAME}
    PRIVATE
        ${${PROJECT_NAME}_INCLUDES}
        ${PROJECT_NAME}/App.cpp
        ${PROJECT_NAME}/Drawer.cpp
        ${PROJECT_NAME}/Shader.cpp
)

find_package(Dawn REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC dawn::webgpu_dawn glfw glm)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)
