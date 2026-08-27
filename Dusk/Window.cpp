module;

#include <GLFW/glfw3.h>

export module Dusk.Window;

import std;

namespace Dusk {

struct GLFWwindowDeleter {
  void operator()(GLFWwindow* window) const {
    if (window) {
      glfwDestroyWindow(window);
    }
  }
};

export using UniqueGLFWwindow = std::unique_ptr<GLFWwindow, GLFWwindowDeleter>;

export class Window {
 private:
  UniqueGLFWwindow _window;

 public:
  explicit Window(UniqueGLFWwindow window) : _window(std::move(window)) {};
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;
  Window(Window&&) noexcept = default;
};

}  // namespace Dusk
