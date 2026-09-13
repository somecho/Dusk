module;

#include <webgpu/webgpu_cpp.h>

export module Dusk.GPU:RenderPipelineBuilder;

namespace Dusk::GPU {
export class RenderPipelineBuilder {
 public:
  RenderPipelineBuilder() {}

  /**
     Sets whether the render pipeline has a fragment shader stage.
     @default true
   */
  auto fragmentState(bool val = true) -> RenderPipelineBuilder& {
    _hasFragmentState = val;
    return *this;
  }

  auto vertexBuffers(size_t numVertexBuffers) -> RenderPipelineBuilder& {
    _desc.vertex.bufferCount = numVertexBuffers;
    return *this;
  }

  auto vertexShaderModule(wgpu::ShaderModule module) -> RenderPipelineBuilder& {
    _desc.vertex.module = module;
    return *this;
  }

  auto bufferLayouts(const wgpu::VertexBufferLayout* layouts)
      -> RenderPipelineBuilder& {
    _desc.vertex.buffers = layouts;
    return *this;
  }

  auto bindGroupLayouts(const wgpu::BindGroupLayout* layouts)
      -> RenderPipelineBuilder& {
    _layoutDesc.bindGroupLayoutCount = 1;
    _layoutDesc.bindGroupLayouts = layouts;
    return *this;
  }

  auto colorTargets(size_t numTargets) -> RenderPipelineBuilder& {
    _colorTargets.assign(numTargets, wgpu::ColorTargetState());
    _fragmentState.targetCount = numTargets;
    _fragmentState.targets = _colorTargets.data();
    return *this;
  }

  auto colorTargetFormat(size_t i, wgpu::TextureFormat format)
      -> RenderPipelineBuilder& {
    _colorTargets.at(i).format = format;
    return *this;
  }

  auto fragmentShaderModule(wgpu::ShaderModule module)
      -> RenderPipelineBuilder& {
    _fragmentState.module = module;
    return *this;
  }

  auto build(const wgpu::Device& device) -> wgpu::RenderPipeline {
    if (_hasFragmentState) {
      _desc.fragment = &_fragmentState;
    }
    _desc.multisample.count = 4;
    _desc.multisample.alphaToCoverageEnabled = false;
    _desc.multisample.mask = ~0u;
    if (_layoutDesc.bindGroupLayouts) {
      _desc.layout = device.CreatePipelineLayout(&_layoutDesc);
    }
    return device.CreateRenderPipeline(&_desc);
  }

 private:
  bool _hasFragmentState = true;
  wgpu::FragmentState _fragmentState = {};
  std::vector<wgpu::ColorTargetState> _colorTargets;
  wgpu::PipelineLayoutDescriptor _layoutDesc{};
  wgpu::RenderPipelineDescriptor _desc;
};
}  // namespace Dusk::GPU
