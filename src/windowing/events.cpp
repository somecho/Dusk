module;

#include <GLFW/glfw3.h>

export module Dusk.windowing:events;

import std;
import :Event;
import :callbacks;

namespace Dusk::windowing {

// Global Event Callback Maps

//
std::map<GLFWwindow*, MouseButtonCallback> mouseDownCallbacks;
std::map<GLFWwindow*, MouseButtonCallback> mouseUpCallbacks;

template <typename F>
auto setCallback(GLFWwindow* win, std::map<GLFWwindow*, F>& map, F cb) -> void {
  map.insert_or_assign(win, cb);
}

export auto setMouseDownCallback(GLFWwindow* win, MouseButtonCallback cb)
    -> void {
  setCallback<MouseButtonCallback>(win, mouseDownCallbacks, cb);
}

export auto setMouseUpCallback(GLFWwindow* win, MouseButtonCallback cb)
    -> void {
  setCallback<MouseButtonCallback>(win, mouseUpCallbacks, cb);
}

}  // namespace Dusk::windowing

namespace Dusk::windowing::EventDispatch {

/**
 * Dispatches MouseDown and MouseUp events.
 */
auto mouseButtonDispatchCallback(GLFWwindow* win, int button, int action,
                                 int mods) -> void {
  MouseButton b = MouseButton::Left;
  switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
      b = MouseButton::Left;
      break;
    case GLFW_MOUSE_BUTTON_RIGHT:
      b = MouseButton::Right;
      break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
      b = MouseButton::Middle;
      break;
  }
  double x, y;
  glfwGetCursorPos(win, &x, &y);
  if (action == GLFW_PRESS) {
    mouseDownCallbacks.at(win)(x, y, b);
  } else if (action == GLFW_RELEASE) {
    mouseUpCallbacks.at(win)(x, y, b);
  }
}

/**
 * Hooks up the window to GLFW's callback system and populates the
 * global callback maps with no-ops.
 */
export auto init(GLFWwindow* const win) {
  glfwSetMouseButtonCallback(win, mouseButtonDispatchCallback);
  setMouseDownCallback(win, [](double, double, MouseButton) {});
  setMouseUpCallback(win, [](double, double, MouseButton) {});
}

}  // namespace Dusk::windowing::EventDispatch
