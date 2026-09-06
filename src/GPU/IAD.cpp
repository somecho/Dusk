module;

#include <webgpu/webgpu_cpp.h>

export module Dusk.GPU:IAD;

import std;

namespace Dusk::GPU {
/**
 * Instance, adapter and device stack.
 */
export class IAD {
 public:
  /**
   * @throws if fails to create a wgpu::Instance
   */
  IAD() {
    createInstance();
    requestAdapter();
    requestDevice();
  }

  auto instance() -> const wgpu::Instance& {
    return _instance;
  }

  auto adapter() -> const wgpu::Adapter& {
    return _adapter;
  }

  auto device() -> const wgpu::Device& {
    return _device;
  }
 private:
  wgpu::Instance _instance;
  wgpu::Adapter _adapter;
  wgpu::Device _device;

  auto createInstance() -> void {
    static constexpr auto kTimedWaitAny =
        wgpu::InstanceFeatureName::TimedWaitAny;
    wgpu::InstanceDescriptor instanceDesc{.requiredFeatureCount = 1,
                                          .requiredFeatures = &kTimedWaitAny};
    _instance = wgpu::CreateInstance(&instanceDesc);
    if (_instance == nullptr) {
      throw std::runtime_error("Failed to create instance");
    }
  }

  auto requestAdapter() -> void {
    wgpu::RequestAdapterOptions options = {};
    auto cb = [](wgpu::RequestAdapterStatus status, wgpu::Adapter adapter,
                 wgpu::StringView message, void* userdata) {
      if (status != wgpu::RequestAdapterStatus::Success) {
        std::cerr << message.data << "\n";
        throw std::runtime_error("Failed to request adapter");
      }
      *static_cast<wgpu::Adapter*>(userdata) = adapter;
    };
    _instance.WaitAny(
        _instance.RequestAdapter(&options, wgpu::CallbackMode::WaitAnyOnly, cb,
                                 (void*)&_adapter),
        UINT64_MAX);
  }

  auto requestDevice() -> void {
    wgpu::DeviceDescriptor desc{};
    desc.SetUncapturedErrorCallback([](const wgpu::Device&,
                                       wgpu::ErrorType type,
                                       wgpu::StringView message) {
      std::string errorTypeName = "";
      switch (type) {
        case wgpu::ErrorType::Validation:
          errorTypeName = "Validation";
          break;
        case wgpu::ErrorType::OutOfMemory:
          errorTypeName = "Out of memory";
          break;
        case wgpu::ErrorType::Internal:
          errorTypeName = "Internal";
          break;
        case wgpu::ErrorType::Unknown:
          errorTypeName = "Unknown";
          break;
        default:
          std::unreachable();
      }
      std::print("{} {}\n", errorTypeName, std::string(message));
    });

    _instance.WaitAny(
        _adapter.RequestDevice(
            &desc, wgpu::CallbackMode::WaitAnyOnly,
            [](wgpu::RequestDeviceStatus status, wgpu::Device device,
               wgpu::StringView message, void* userdata) {
              if (status != wgpu::RequestDeviceStatus::Success) {
                std::cerr << std::string(message) << "\n";
                throw std::runtime_error("Failed to request device");
              }
              *static_cast<wgpu::Device*>(userdata) = device;
            },
            &_device),
        UINT64_MAX);

#ifndef __EMSCRIPTEN__
    _device.SetLoggingCallback(
        [](wgpu::LoggingType type, wgpu::StringView message) {
          std::string loggingType = "";
          switch (type) {
            case wgpu::LoggingType::Verbose:
              loggingType = "Verbose";
              break;
            case wgpu::LoggingType::Info:
              loggingType = "Info";
              break;
            case wgpu::LoggingType::Warning:
              loggingType = "Warning";
              break;
            case wgpu::LoggingType::Error:
              loggingType = "Error";
              break;
          }
          std::print("{} {} \n", loggingType, std::string(message));
        });
#endif
  }
};

}  // namespace Dusk::GPU
