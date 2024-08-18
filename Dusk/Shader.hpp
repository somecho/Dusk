#pragma once

#include <webgpu/webgpu_cpp.h>

namespace Dusk {

struct Shader {
  wgpu::ShaderModule mod;
};

class ShaderBuilder {
 public:
  ShaderBuilder& source(const char* source);
  Shader build(const wgpu::Device& device);

 private:
  const char* shaderSource;
};

}  // namespace Dusk