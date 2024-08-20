#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Dusk {
namespace Drawable {
namespace Interface {

template <typename Derived>
class Position {
 public:
  Derived& xy(float x, float y) {
    pos.x = x;
    pos.y = y;
    return static_cast<Derived&>(*this);
  }

  Derived& xy(glm::vec2 pos) {
    this->pos = pos;
    return static_cast<Derived&>(*this);
  }

  glm::vec2 xy() {
    return pos;
  }

  Derived& x(float x) {
    pos.x = x;
    return static_cast<Derived&>(*this);
  };

  float x() {
    return pos.x;
  }

  Derived& y(float y) {
    pos.y = y;
    return static_cast<Derived&>(*this);
  }

  float y() {
    return pos.y;
  }

 private:
  glm::vec2 pos{0, 0};
};

template <typename Derived>
class Dimensions {
 public:
  Derived& w(float width) {
    this->width = width;
    return static_cast<Derived&>(*this);
  }

  float w() {
    return width;
  }

  Derived& h(float height) {
    this->width = height;
    return static_cast<Derived&>(*this);
  }

  float h() {
    return height;
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

  glm::vec4 rgba() {
    return color;
  }

  Derived& value(float value, float alpha = 1.0) {
    color.r = value;
    color.g = value;
    color.b = value;
    return static_cast<Derived&>(*this);
  }

  Derived& rgb(float r, float g, float b) {
    color.r = r;
    color.g = g;
    color.b = b;
    return static_cast<Derived&>(*this);
  }

  glm::vec3 rgb() {
    return glm::vec3(color);
  }

 private:
  glm::vec4 color{1, 1, 1, 1};
};

}  // namespace Interface
}  // namespace Drawable
}  // namespace Dusk