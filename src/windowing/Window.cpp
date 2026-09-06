module;

#include <GLFW/glfw3.h>

export module Dusk.windowing:Window;

import std;
import :events;

namespace Dusk::windowing {

struct GLFWwindowDeleter {
  void operator()(GLFWwindow* window) const {
    if (window) {
      glfwDestroyWindow(window);
    }
  }
};

export using UniqueGLFWwindow = std::unique_ptr<GLFWwindow, GLFWwindowDeleter>;

export class Window {
 public:
  explicit Window(UniqueGLFWwindow window) : _window(std::move(window)) {
    glfwGetWindowSize(_window.get(), &_width, &_height);
    EventDispatch::init(_window.get());
  };
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;
  Window(Window&&) noexcept = default;

  auto shouldClose() -> bool {
    return glfwWindowShouldClose(_window.get());
  }

  auto close() -> void {
    glfwSetWindowShouldClose(_window.get(), 1);
  }

  auto get() -> GLFWwindow* {
    return _window.get();
  }

  auto width() -> int {
    return _width;
  }
  auto height() -> int {
    return _height;
  }

  /**
   * Assigns a callback to an event fired in the current window.
   *
   * EventCallback signature should match event.
   *
   * Event-Signature mappings:
   * - MouseDown - MouseButtonCallback
   * - MouseUp - MouseButtonCallback
   */
  auto setEventCallback(Event event, EventCallback callback) -> void {
    switch (event) {
      case Event::MouseDown:
        setMouseDownCallback(_window.get(),
                             std::get<MouseButtonCallback>(callback));
        break;
      case Event::MouseUp:
        setMouseUpCallback(_window.get(),
                           std::get<MouseButtonCallback>(callback));
        break;
      default:
        std::unreachable();
    }
  }

 private:
  UniqueGLFWwindow _window;
  int _width, _height;
};

}  // namespace Dusk::windowing
