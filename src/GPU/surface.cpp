module;

#include <webgpu/webgpu_cpp.h>

#ifdef __EMSCRIPTEN__
#else
#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_X11
#include "GLFW/glfw3native.h"
#endif

export module Dusk.GPU:surface;

namespace Dusk::GPU {

#ifdef __EMSCRIPTEN__

export auto CreateSurfaceForWindow(const wgpu::Instance& instance,
                                   void* = nullptr) -> wgpu::Surface {
  wgpu::EmscriptenSurfaceSourceCanvasHTMLSelector fromCanvasHTMLSelector{};
  fromCanvasHTMLSelector.selector = wgpu::StringView("canvas");
  wgpu::SurfaceDescriptor descriptor{};
  descriptor.nextInChain = &fromCanvasHTMLSelector;
  wgpu::Surface surface = instance.CreateSurface(&descriptor);
  return surface;
}

#else

using UniqueChainedStruct =
    std::unique_ptr<wgpu::ChainedStruct, void (*)(wgpu::ChainedStruct*)>;

export auto CreateSurfaceForWindow(const wgpu::Instance& instance,
                                   GLFWwindow* window) {
  wgpu::SurfaceSourceXlibWindow* desc = new wgpu::SurfaceSourceXlibWindow();
  desc->display = glfwGetX11Display();
  desc->window = glfwGetX11Window((window));
  wgpu::SurfaceDescriptor descriptor;
  descriptor.nextInChain = desc;
  wgpu::Surface surface = instance.CreateSurface(&descriptor);
  delete desc;
  return surface;
}
#endif

}  // namespace Dusk::GPU
