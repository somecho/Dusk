#pragma once

#include <Dusk/Drawable/Interface.hpp>

namespace Dusk {
namespace Drawable {

class Rect : public Interface::Position<Rect>,
             public Interface::Dimensions<Rect>,
             public Interface::Color<Rect> {};

}  // namespace Drawable

}  // namespace Dusk