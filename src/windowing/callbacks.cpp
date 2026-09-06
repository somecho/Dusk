export module Dusk.windowing:callbacks;

import std;
import :Event;

namespace Dusk::windowing {

export using MouseButtonCallback =
    std::function<void(double, double, MouseButton)>;

export using EventCallback = std::variant<MouseButtonCallback>;

}  // namespace Dusk::windowing
