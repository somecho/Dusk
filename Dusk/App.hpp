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
  void run();
  virtual void onKeypressed([[maybe_unused]] int key,
                            [[maybe_unused]] int action) {};

  inline int getWidth() {
    return width;
  }

  inline int getHeight() {
    return height;
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
};

}  // namespace Dusk