module;

#include <webgpu/webgpu_cpp.h>

#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_X11
#include "GLFW/glfw3native.h"

export module Dusk.GPU:glfw;

namespace Dusk::GPU {

using UniqueChainedStruct =
    std::unique_ptr<wgpu::ChainedStruct, void (*)(wgpu::ChainedStruct*)>;

/**
   Creates a surface for X11 (GLFW).
 */
export auto CreateSurfaceForWindow(const wgpu::Instance& instance,
                                   GLFWwindow* window) {
  wgpu::SurfaceSourceXlibWindow* desc = new wgpu::SurfaceSourceXlibWindow();
  desc->display = glfwGetX11Display();
  desc->window = glfwGetX11Window(window);
  wgpu::SurfaceDescriptor descriptor;
  descriptor.nextInChain = desc;
  wgpu::Surface surface = instance.CreateSurface(&descriptor);
  delete desc;
  return surface;
}

}  // namespace Dusk::GPU
