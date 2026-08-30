module;

#include <GLFW/glfw3.h>

export module Dusk.windowing:Window;

import std;

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

 private:
  UniqueGLFWwindow _window;
  int _width, _height;
};

}  // namespace Dusk
