module;

#include <GLFW/glfw3.h>

export module Dusk.wf;

export import Dusk.WindowBuilder;

namespace Dusk {

/**
   Windowing framework
 */
export class wf {
 private:
  inline static bool _initialized = false;

 public:
  wf() = delete;

  static auto init() -> void {
    if (glfwInit() == GLFW_TRUE) {
      _initialized = true;
    }
  }

  static auto shutdown() -> void {
    glfwTerminate();
  }

  static auto initialized() -> bool {
    return _initialized;
  }

  static auto Window() -> Dusk::WindowBuilder {
    return Dusk::WindowBuilder();
  }
};

}  // namespace Dusk
