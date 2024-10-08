#pragma once

#include <cstdint>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <tuple>

template <typename T>
struct always_false : std::false_type {};

namespace Dusk {

typedef std::tuple<float, float, float> Triplet;

namespace Drawable {
namespace Interface {

class Gettable {
 public:
  virtual ~Gettable() = default;

  template <typename T>
  T getMember(const glm::vec3& member) {
    if constexpr (std::is_same_v<T, glm::vec3>) {
      return member;
    } else if constexpr (std::is_same_v<T, Triplet>) {
      return Triplet{member.x, member.y, member.z};
    } else {
      static_assert(always_false<T>::value, "Unsupported type");
    }
  }
};

template <typename Derived>
class Position {
 public:
  virtual ~Position<Derived>() = default;

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
  virtual ~Dimensions<Derived>() = default;

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
  virtual ~Color<Derived>() = default;

  Derived& rgba(float r, float g, float b, float a = 1.0) {
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    return static_cast<Derived&>(*this);
  }

  Derived& rgba(float value, float alpha = 1.0) {
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
  virtual ~Radius<Derived>() = default;

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
  virtual ~Resolution<Derived>() = default;

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

template <typename Derived>
class Triangle : public Gettable {
 public:
  virtual ~Triangle<Derived>() = default;

  Derived& p1(glm::vec2 pos) {
    return p1(pos.x, pos.y);
  }

  Derived& p1(glm::vec3 pos) {
    return p1(pos.x, pos.y, pos.z);
  }

  Derived& p1(float x, float y, float z = 0) {
    v1.x = x;
    v1.y = y;
    v1.z = z;
    return static_cast<Derived&>(*this);
  }
  Derived& p2(glm::vec2 pos) {
    return p2(pos.x, pos.y);
  }

  Derived& p2(glm::vec3 pos) {
    return p2(pos.x, pos.y, pos.z);
  }

  Derived& p2(float x, float y, float z = 0) {
    v2.x = x;
    v2.y = y;
    v2.z = z;
    return static_cast<Derived&>(*this);
  }

  Derived& p3(glm::vec2 pos) {
    return p3(pos.x, pos.y);
  }

  Derived& p3(glm::vec3 pos) {
    return p3(pos.x, pos.y, pos.z);
  }

  Derived& p3(float x, float y, float z = 0) {
    v3.x = x;
    v3.y = y;
    v3.z = z;
    return static_cast<Derived&>(*this);
  }

  template <typename T>
  T p1() {
    return getMember<T>(v1);
  }

  template <typename T>
  T p2() {
    return getMember<T>(v2);
  }

  template <typename T>
  T p3() {
    return getMember<T>(v3);
  }

 private:
  glm::vec3 v1;
  glm::vec3 v2;
  glm::vec3 v3;
};

template <typename Derived>
class Line : public Gettable {
 public:
  virtual ~Line<Derived>() = default;

  Derived& p1(glm::vec2 pos) {
    return p1(pos.x, pos.y);
  }

  Derived& p1(glm::vec3 pos) {
    return p1(pos.x, pos.y, pos.z);
  }

  Derived& p1(float x, float y, float z = 0) {
    v1.x = x;
    v1.y = y;
    v1.z = z;
    return static_cast<Derived&>(*this);
  }
  Derived& p2(glm::vec2 pos) {
    return p2(pos.x, pos.y);
  }

  Derived& p2(glm::vec3 pos) {
    return p2(pos.x, pos.y, pos.z);
  }

  Derived& p2(float x, float y, float z = 0) {
    v2.x = x;
    v2.y = y;
    v2.z = z;
    return static_cast<Derived&>(*this);
  }

  template <typename T>
  T p1() {
    return getMember<T>(v1);
  }

  template <typename T>
  T p2() {
    return getMember<T>(v2);
  }

 private:
  glm::vec3 v1;
  glm::vec3 v2;
};

template <typename Derived>
class Thickness {
 public:
  virtual ~Thickness<Derived>() = default;

  Derived& thickness(float t) {
    this->t = t;
    return static_cast<Derived&>(*this);
  }

  float thickness() {
    return t;
  }

 private:
  float t;
};

}  // namespace Interface
}  // namespace Drawable
}  // namespace Dusk