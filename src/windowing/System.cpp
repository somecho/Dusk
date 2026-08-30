module;

#include <GLFW/glfw3.h>

export module Dusk.windowing:System;
export import :WindowBuilder;
import std;

namespace Dusk::windowing {

export class System {
 public:
  System() {
    if (glfwInit() != GLFW_TRUE) {
      throw std::runtime_error("Failed to initialize GLFW");
    }
  }
  ~System() {
    glfwTerminate();
  }

  auto window() -> WindowBuilder {
    return WindowBuilder();
  }

  auto pollEvents() -> void {
    glfwPollEvents();
  }
};

}  // namespace Dusk::windowing
