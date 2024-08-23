#pragma once

#include <Dusk/Interface.hpp>
#include <variant>

namespace Dusk {
namespace Drawable {

class Rect : public Interface::Position<Rect>,
             public Interface::Dimensions<Rect>,
             public Interface::Color<Rect> {};

class Circle : public Interface::Position<Circle>,
               public Interface::Radius<Circle>,
               public Interface::Resolution<Circle>,
               public Interface::Color<Circle> {};

class Ellipse : public Interface::Position<Ellipse>,
                public Interface::Dimensions<Ellipse>,
                public Interface::Resolution<Ellipse>,
                public Interface::Color<Ellipse> {};

class Triangle : public Interface::Triangle<Triangle>,
                 public Interface::Color<Triangle> {};

typedef std::variant<Rect, Circle, Ellipse, Triangle> Shape;

}  // namespace Drawable
}  // namespace Dusk