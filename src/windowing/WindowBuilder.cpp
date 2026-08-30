module;

#include <GLFW/glfw3.h>

export module Dusk.windowing:WindowBuilder;

export import :Window;

import std;


namespace Dusk::windowing {

export class WindowBuilder {
 private:
  int _width = 1280;
  int _height = 800;
  std::string _title = "Dusk";

 public:
  WindowBuilder() {}
  WindowBuilder(const WindowBuilder&) = delete;
  WindowBuilder& operator=(const WindowBuilder&) = delete;
  WindowBuilder(WindowBuilder&&) noexcept = default;

  auto withDimensions(int width, int height) -> WindowBuilder& {
    _width = width;
    _height = height;
    return *this;
  }

  auto withTitle(std::string title) -> WindowBuilder& {
    _title = title;
    return *this;
  }

  auto create() -> std::optional<Window> {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    auto window =
        glfwCreateWindow(_width, _height, _title.c_str(),
                         nullptr /*TODO: allow monitor to be set*/, nullptr);
    if (!window) {
      return {};
    }
    return Window(UniqueGLFWwindow(window));
  }
};

}  // namespace Dusk
