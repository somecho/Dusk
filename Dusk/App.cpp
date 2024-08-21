#include <Dusk/App.hpp>
#include <Dusk/Drawer.hpp>
#include <format>
#include <glm/ext/matrix_transform.hpp>
// do not change this order
// WebGPU headers must be included before this define
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>

#include <iostream>

#define PRINT(status, type, msg) \
  std::cout << "[" << status << "] " << type << " - " << msg << std::endl
#define LOG_WGPU(msg) PRINT("INFO", "Dawn::WebGPU", msg)
#define LOG_GLFW(msg) PRINT("INFO", "GLFW", msg)
#define ERR_WGPU(msg) PRINT("ERROR", "Dawn::WebGPU", msg)
#define ERR_GLFW(msg) PRINT("ERROR", "GLFW", msg)
#define SUCCESS_WGPU(msg) PRINT("SUCCESS", "Dawn::WebGPU", msg)
#define SUCCESS_GLFW(msg) PRINT("SUCCESS", "GLFW", msg)

namespace Dusk {

App::App() {
  LOG_GLFW("Initializing GLFW...");
  glfwInitialized = glfwInit();
  if (!glfwInitialized) {
    std::cerr << "Unable to initialize GLFW." << std::endl;
    std::exit(1);
  }
  SUCCESS_GLFW("Successfully initialized GLFW");
  createInstance();
}

App::~App() {
  updateThread.join();

  LOG_WGPU("Releasing WebGPU resources");
  surface = nullptr;
  device = nullptr;
  adapter = nullptr;
  instance = nullptr;
  SUCCESS_WGPU("Successfully released WebGPU resources");

  LOG_GLFW("Terminating GLFW...");
  if (glfwInitialized) {
    glfwDestroyWindow(window);
    glfwTerminate();
  }
  SUCCESS_GLFW("Successfully terminated GLFW");
}

void App::run(int width, int height) {
  this->width = width;
  this->height = height;
  run();
}

void App::run() {
  LOG_GLFW("Creating window...");
  static App *appInstance = this;
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window = glfwCreateWindow(width, height, "GLFW", nullptr, nullptr);
  SUCCESS_GLFW("Successfully created window");
  glfwSetKeyCallback(
      window, [](GLFWwindow *window, int key, [[maybe_unused]] int scancode,
                 int action, [[maybe_unused]] int mods) {
        if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
          glfwSetWindowShouldClose(window, 1);
        }
        appInstance->onKeypressed(key, action);
      });

  createSurface();
  requestAdapter();
  requestDevice();
  configureSurface();
  {
    queue = device.GetQueue();
    if (!queue) {
      ERR_WGPU("Unable to get queue");
    }
    SUCCESS_WGPU("Successfully gotten queue");
  }

  wgpu::SurfaceCapabilities caps;
  surface.GetCapabilities(adapter, &caps);

  drawer = Dusk::Drawer(device, surface, caps.formats[0]);

  glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(-1, 1, 0));
  glm::mat4 scale = glm::scale(
      glm::mat4(1.0f),
      glm::vec3(2.0 / (float)getWidth(), -2.0 / (float)getHeight(), 1));

  drawer.setTransformMatrix(translate * scale);

  setup();

  updateThread = std::thread([this]() {
    while (!glfwWindowShouldClose(window)) {
      update();
    }
  });

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    instance.ProcessEvents();
    draw();
    surface.Present();
    frameNum++;
  }
}

void App::createInstance() {
  LOG_WGPU("Creating WebGPU Instance...");

  wgpu::InstanceDescriptor desc{};
  desc.nextInChain = nullptr;
  desc.features.timedWaitAnyEnable = true;
  instance = wgpu::CreateInstance(&desc);

  if (!instance) {
    ERR_WGPU("Unable to create a WebGPU Instance");
    std::exit(1);
  }

  SUCCESS_WGPU("Successfully created a WebGPU Instance");
}

void App::createSurface() {
  LOG_WGPU("Creating surface...");

  wgpu::SurfaceDescriptorFromXlibWindow xlibDesc{};
  xlibDesc.nextInChain = nullptr;
  xlibDesc.sType = wgpu::SType::SurfaceDescriptorFromXlibWindow;
  xlibDesc.display = glfwGetX11Display();
  xlibDesc.window = glfwGetX11Window(window);

  wgpu::SurfaceDescriptor surfaceDesc;
  surfaceDesc.nextInChain = &xlibDesc;
  surfaceDesc.label = nullptr;

  surface = instance.CreateSurface(&surfaceDesc);

  if (!surface) {
    ERR_WGPU("Unable to create a surface");
    std::exit(1);
  }

  SUCCESS_WGPU("Succesfully created surface");
}

void App::requestAdapter() {
  wgpu::RequestAdapterOptions adapterOpts{};
  adapterOpts.compatibleSurface = surface;

  wgpu::RequestAdapterCallbackInfo callbackInfo = {};
  callbackInfo.nextInChain = nullptr;
  callbackInfo.mode = wgpu::CallbackMode::WaitAnyOnly;
  callbackInfo.userdata = &adapter;
  callbackInfo.callback = []([[maybe_unused]] WGPURequestAdapterStatus status,
                             WGPUAdapter adapter,
                             [[maybe_unused]] const char *message,
                             void *userdata) {
    if (status == WGPURequestAdapterStatus_Success) {
      SUCCESS_WGPU("Adapter acquired");
      *static_cast<wgpu::Adapter *>(userdata) = wgpu::Adapter::Acquire(adapter);
    } else {
      ERR_WGPU(std::format("Unable to create device: {}", message));
      std::exit(1);
    }
  };

  LOG_WGPU("Requesting adapter...");
  instance.WaitAny(instance.RequestAdapter(&adapterOpts, callbackInfo),
                   UINT64_MAX);
}

void App::requestDevice() {
  wgpu::DeviceDescriptor deviceDesc{};

  wgpu::RequestDeviceCallbackInfo deviceCallbackInfo{};
  deviceCallbackInfo.userdata = &device;
  deviceCallbackInfo.nextInChain = nullptr;
  deviceCallbackInfo.mode = wgpu::CallbackMode::WaitAnyOnly;
  deviceCallbackInfo.callback = [](WGPURequestDeviceStatus status,
                                   WGPUDevice device, const char *message,
                                   void *userdata) {
    if (status == WGPURequestDeviceStatus_Success) {
      SUCCESS_WGPU("Device acquired");
      *static_cast<wgpu::Device *>(userdata) = wgpu::Device::Acquire(device);
    } else {
      ERR_WGPU(std::format("Unable to create device: {}", message));
      std::exit(1);
    }
  };

  LOG_WGPU("Requesting device...");
  instance.WaitAny(adapter.RequestDevice(&deviceDesc, deviceCallbackInfo),
                   UINT64_MAX);
}

void App::configureSurface() {
  LOG_WGPU("Configuring surface...");

  wgpu::SurfaceCapabilities caps;
  surface.GetCapabilities(adapter, &caps);

  wgpu::SurfaceConfiguration config = {};
  config.nextInChain = nullptr;
  config.device = device;
  config.width = width;
  config.height = height;
  config.format = caps.formats[0];
  config.usage = wgpu::TextureUsage::RenderAttachment;
  config.presentMode = wgpu::PresentMode::Fifo;
  config.viewFormatCount = 0;
  config.viewFormats = nullptr;

  surface.Configure(&config);
  SUCCESS_WGPU("Successfully configured surface");
}

}  // namespace Dusk