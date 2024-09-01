#pragma once

#include <GLFW/glfw3.h>
#include <webgpu/webgpu_cpp.h>

#include <Dusk/Drawer.hpp>
#include <cstdint>
#include <thread>

namespace Dusk {

class App {
 public:
  App();
  virtual ~App();

  void run(int width, int height);
  void run();

  virtual void onKeyPressed([[maybe_unused]] int key,
                            [[maybe_unused]] int action) {};

  virtual void onMouseMoved([[maybe_unused]] double mouseX,
                            [[maybe_unused]] double mouseY) {};

  virtual void onMousePressed([[maybe_unused]] double mouseX,
                              [[maybe_unused]] double mouseY,
                              [[maybe_unused]] int button) {};

  virtual void onMouseReleased([[maybe_unused]] double mouseX,
                               [[maybe_unused]] double mouseY,
                               [[maybe_unused]] int button) {};

  virtual void onMouseDragged([[maybe_unused]] double mouseX,
                              [[maybe_unused]] double mouseY) {};

  inline int getWidth() {
    return width;
  }

  inline int getHeight() {
    return height;
  }

  inline glm::vec2 getCenter() {
    return {width * 0.5, height * 0.5};
  }

 protected:
  wgpu::Instance instance;
  wgpu::Surface surface;
  wgpu::Adapter adapter;
  wgpu::Device device;
  wgpu::Queue queue;
  Dusk::Drawer drawer;

 private:
  int width = 1280;
  int height = 720;
  uint64_t frameNum = 0;
  int glfwInitialized = false;
  GLFWwindow* window;
  std::thread updateThread;

  void createInstance();
  void createSurface();
  void requestAdapter();
  void requestDevice();
  void configureSurface();

  virtual void setup() {};
  virtual void update() {};
  virtual void draw() {};

  bool mousePressed = false;
};

}  // namespace Dusk