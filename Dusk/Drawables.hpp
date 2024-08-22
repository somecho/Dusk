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
               public Interface::Color<Rect> {};

typedef std::variant<Rect, Circle> Shape;

}  // namespace Drawable
}  // namespace Dusk