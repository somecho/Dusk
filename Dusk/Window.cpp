module;

#include <GLFW/glfw3.h>

/**
   Windowing library using GLFW
 */
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

using UniqueGLFWwindow = std::unique_ptr<GLFWwindow, GLFWwindowDeleter>;

export class Window {
 private:
  explicit Window(UniqueGLFWwindow window, int width, int height,
                  std::string title)
      : _window(std::move(window)),
        _width(width),
        _height(height),
        _title(title) {
    Library::log(std::format("{} Window created\n", _tag));
    Library::log(std::format("{} Title: {}\n", _tag, _title));
    Library::log(std::format("{} Dimensions: {}x{}\n", _tag, _width, _height));
  }

  UniqueGLFWwindow _window;
  int _width, _height;
  std::string _title;
  const std::string _tag = "Dusk::Window";

 public:
  // disable copy contructors
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  // default move constructors
  Window(Window&&) noexcept = default;

  class Library {
   private:
    static constexpr std::string _tag = "Dusk::Window::Library";

   public:
    Library() = delete;

    static auto initialize() -> void {
      log(std::format("{} Initializing GLFW\n", _tag));
      if (glfwInit() != GLFW_TRUE) {
        onError(std::format("{} Unable to initialize GLFW\n", _tag));
        logGlfwError(_tag);
      };
      log(std::format("{} GLFW Initialized\n", _tag));
    }

    static auto shutdown() -> void {
      /**
         No need to check if GLFW has been initialized since
         `glfwTerminate` has no effect if not initialized.

         Info:
         https://www.glfw.org/docs/latest/group__init.html#gaaae48c0a18607ea4a4ba951d939f0901
       */
      glfwTerminate();
      log(std::format("{} Terminating GLFW\n", _tag));
    }

    static auto setOnError(std::function<void(std::string)> fn) -> void {
      onError = fn;
    }

    static auto setLog(std::function<void(std::string)> fn) -> void {
      log = fn;
    }
    static auto logGlfwError(std::string_view tag) -> void {
      const char* error;
      int code = glfwGetError(&error);
      onError(std::format("{} GLFW Error Code: {}\n", tag, code));
      onError(
          std::format("{} GLFW Error Message: {}\n", tag, std::string(error)));
    }

    static inline std::function<void(std::string)> onError = [](std::string) {};
    static inline std::function<void(std::string)> log = [](std::string) {};
  };

  class Builder {
   private:
    int _width = 1280;
    int _height = 800;
    std::string _title = "Dusk";
    std::string _tag = "Dusk::Window::Builder";

   public:
    auto width(int width) -> Builder& {
      this->_width = width;
      return *this;
    }
    auto height(int height) -> Builder& {
      this->_height = height;
      return *this;
    }
    auto title(std::string title) -> Builder& {
      this->_title = title;
      return *this;
    }

    auto create() -> std::optional<Dusk::Window> {
      Library::log(std::format("{} Creating window\n", _tag));
      /**
         Not using OpenGL so setting to GLFW NO API.
         TODO: Allow configure via builder
       */
      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

      auto window =
          glfwCreateWindow(_width, _height, _title.c_str(),
                           nullptr /*TODO: allow monitor to be set*/, nullptr);
      if (window == nullptr) {
        Library::onError(std::format("{} Unable to create window\n", _tag));
        Library::logGlfwError(_tag);
        return {};
      }
      return Dusk::Window(UniqueGLFWwindow(window), _width, _height, _title);
    }
  };
};

};  // namespace Dusk
