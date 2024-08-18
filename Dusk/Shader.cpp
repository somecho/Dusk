#include <Dusk/Shader.hpp>

namespace Dusk {

 ShaderBuilder& ShaderBuilder::source(const char* source) {
    shaderSource = source;
    return *this;
  }

  Shader ShaderBuilder::build(const wgpu::Device& device) {
    wgpu::ShaderModuleWGSLDescriptor wgslDesc;
    wgslDesc.nextInChain = nullptr;
    wgslDesc.sType = wgpu::SType::ShaderModuleWGSLDescriptor;
    wgslDesc.code = shaderSource;

    wgpu::ShaderModuleDescriptor shaderDesc;
    shaderDesc.nextInChain = &wgslDesc;

    return {device.CreateShaderModule(&shaderDesc)};
  }

}