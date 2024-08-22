#pragma once

#include <cstdint>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Dusk {
namespace Drawable {
namespace Interface {

template <typename Derived>
class Position {
 public:
  Derived& xyz(float x, float y, float z) {
    pos.x = x;
    pos.y = y;
    pos.z = z;
    return static_cast<Derived&>(*this);
  }

  Derived& xyz(glm::vec3 pos) {
    this->pos = pos;
    return static_cast<Derived&>(*this);
  }

  Derived& xy(float x, float y) {
    pos.x = x;
    pos.y = y;
    return static_cast<Derived&>(*this);
  }

  Derived& xy(glm::vec2 pos) {
    this->pos.x = pos.x;
    this->pos.y = pos.y;
    return static_cast<Derived&>(*this);
  }

  Derived& x(float x) {
    pos.x = x;
    return static_cast<Derived&>(*this);
  };

  Derived& y(float y) {
    pos.y = y;
    return static_cast<Derived&>(*this);
  }

  glm::vec3 xyz() {
    return pos;
  }

  glm::vec2 xy() {
    return pos;
  }

  float x() {
    return pos.x;
  }

  float y() {
    return pos.y;
  }

  float z() {
    return pos.z;
  }

 private:
  glm::vec3 pos{0, 0, 0};
};

template <typename Derived>
class Dimensions {
 public:
  Derived& w(float width) {
    this->width = width;
    return static_cast<Derived&>(*this);
  }

  Derived& h(float height) {
    this->height = height;
    return static_cast<Derived&>(*this);
  }

  Derived& wh(float width, float height) {
    this->width = width;
    this->height = height;
    return static_cast<Derived&>(*this);
  }

  Derived& wh(glm::vec2 dimensions) {
    this->width = dimensions.x;
    this->height = dimensions.y;
    return static_cast<Derived&>(*this);
  }

  float w() {
    return width;
  }

  float h() {
    return height;
  }

  glm::vec2 wh() {
    return glm::vec2(width, height);
  }

 private:
  float width;
  float height;
};

template <typename Derived>
class Color {
 public:
  Derived& rgba(float r, float g, float b, float a = 1.0) {
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    return static_cast<Derived&>(*this);
  }


  Derived& value(float value, float alpha = 1.0) {
    color.r = value;
    color.g = value;
    color.b = value;
    color.a = alpha;
    return static_cast<Derived&>(*this);
  }

  Derived& rgb(float r, float g, float b) {
    color.r = r;
    color.g = g;
    color.b = b;
    return static_cast<Derived&>(*this);
  }

  glm::vec4 rgba() {
    return color;
  }

  glm::vec3 rgb() {
    return glm::vec3(color);
  }

  float r() {
    return color.r;
  }

  float g() {
    return color.g;
  }

  float b() {
    return color.b;
  }

  float a() {
    return color.a;
  }

 private:
  glm::vec4 color{1, 1, 1, 1};
};

template <typename Derived>
class Radius {
 public:
  Derived& radius(float radius) {
    m_radius = radius;
    return static_cast<Derived&>(*this);
  }

  float radius() {
    return m_radius;
  }

 private:
  float m_radius;
};

template <typename Derived>
class Resolution {
 public:
  Derived& res(uint32_t resolution) {
    this->resolution = resolution;
    return static_cast<Derived&>(*this);
  }

  uint32_t res() {
    return resolution;
  }

 private:
  uint32_t resolution = 90;
};

}  // namespace Interface
}  // namespace Drawable
}  // namespace Dusk