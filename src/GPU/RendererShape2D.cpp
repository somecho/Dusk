module;

#include <webgpu/webgpu_cpp.h>

#include <glm/ext.hpp>

export module Dusk.GPU:RendererShape2D;

import :RenderPipelineBuilder;

namespace Dusk {

export class RendererShape2D {
 public:
  RendererShape2D() = default;

  RendererShape2D(
      const wgpu::Device& device, uint32_t width, uint32_t height,
      wgpu::TextureFormat textureFormat = wgpu::TextureFormat::BGRA8Unorm)
      : _width(width), _height(height), _textureFormat(textureFormat) {
    initTransformBuffer(device);
    initColorBuffer(device);
    initTargetTex(device);
    initShaderModule(device);
    initBufferLayout();
    initBindGroups(device);
    _pipeline = Dusk::GPU::RenderPipelineBuilder()
                    .vertexBuffers(1)
                    .vertexShaderModule(_shaderModule)
                    .bufferLayouts(_bufferLayout.data())
                    .colorTargets(1)
                    .colorTargetFormat(0, textureFormat)
                    .fragmentShaderModule(_shaderModule)
                    .bindGroupLayouts(&_bindGroupLayout)
                    .build(device);
  }

  auto render(const wgpu::Device& device, wgpu::Surface& surface,
              const std::vector<float>& vertices,
              const std::array<float, 4> color,
              const std::vector<uint32_t>& indices) {
    // Write Buffers
    std::array<wgpu::BufferDescriptor, 2> bufDesc;
    bufDesc[0].usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst;
    bufDesc[0].size = sizeof(vertices[0]) * vertices.size();
    auto vertexBuffer = device.CreateBuffer(&bufDesc[0]);
    device.GetQueue().WriteBuffer(vertexBuffer, 0, vertices.data(),
                                  bufDesc[0].size);
    bufDesc[1].usage = wgpu::BufferUsage::Index | wgpu::BufferUsage::CopyDst;
    bufDesc[1].size = sizeof(indices[1]) * indices.size();
    auto indexBuffer = device.CreateBuffer(&bufDesc[1]);
    device.GetQueue().WriteBuffer(indexBuffer, 0, indices.data(),
                                  bufDesc[1].size);
    device.GetQueue().WriteBuffer(_colorBuffer, 0, color.data(),
                                  sizeof(float) * 4);

    // Render
    wgpu::SurfaceTexture st;
    surface.GetCurrentTexture(&st);

    wgpu::RenderPassColorAttachment attachment;
    attachment.loadOp = wgpu::LoadOp::Load;
    attachment.storeOp = wgpu::StoreOp::Store;
    attachment.view = _tex.CreateView();
    attachment.resolveTarget = st.texture.CreateView();

    wgpu::RenderPassDescriptor renderPass{};
    renderPass.colorAttachmentCount = 1;
    renderPass.colorAttachments = &attachment;

    auto encoder = device.CreateCommandEncoder();
    auto pass = encoder.BeginRenderPass(&renderPass);
    pass.SetPipeline(_pipeline);
    pass.SetBindGroup(0, _bindGroup);
    pass.SetVertexBuffer(0, vertexBuffer);
    pass.SetIndexBuffer(indexBuffer, wgpu::IndexFormat::Uint32);
    pass.DrawIndexed(indices.size());
    pass.End();
    auto commands = encoder.Finish();
    device.GetQueue().Submit(1, &commands);
  }

  static constexpr std::string_view shader = R"(
        @group(0) @binding(0) var<uniform> transformMat: mat4x4f;
        @group(0) @binding(1) var<uniform> shapeColor: vec4f;

        struct VertexOutput {
          @builtin(position) position: vec4f,
        };

        @vertex fn vs(@location(0) pos : vec2f) -> VertexOutput {
            var out: VertexOutput;
            out.position = transformMat * vec4(pos,0.0, 1.0);
            return out;
        }

        @fragment fn fs(in: VertexOutput) -> @location(0) vec4f {
            return shapeColor;
        }
    )";

 private:
  uint32_t _width, _height;
  wgpu::TextureFormat _textureFormat;
  wgpu::Buffer _transformBuffer; /*< Orthographic projection matrix uniform */
  wgpu::Buffer _colorBuffer;     /*< Color uniform */
  wgpu::Texture _tex;            /*< Target Render Attachment */
  wgpu::ShaderModule _shaderModule;
  std::array<wgpu::VertexBufferLayout, 1> _bufferLayout = {};
  std::array<wgpu::VertexAttribute, 1> _vertexAttribs = {};
  wgpu::BindGroupLayout _bindGroupLayout;
  wgpu::BindGroup _bindGroup;
  wgpu::RenderPipeline _pipeline;

  /**
   * Initialize and populate transform buffer
   */
  auto initTransformBuffer(const wgpu::Device& device) -> void {
    auto transformMatrix = glm::ortho<float>(0, _width, _height, 0, -1, 1);
    wgpu::BufferDescriptor desc{};
    desc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
    desc.size = sizeof(float) * 16;
    desc.label = "RendererShape2D Orthographic Projection Matrix Buffer";
    _transformBuffer = device.CreateBuffer(&desc);
    device.GetQueue().WriteBuffer(_transformBuffer, 0,
                                  glm::value_ptr(transformMatrix), desc.size);
  }

  /**
   * Initialize empty buffer to hold color uniform
   */
  auto initColorBuffer(const wgpu::Device& device) -> void {
    wgpu::BufferDescriptor desc{};
    desc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
    desc.size = sizeof(float) * 4;
    desc.label = "RendererShape2D Color Buffer";
    _colorBuffer = device.CreateBuffer(&desc);
  }

  auto initTargetTex(const wgpu::Device& device) -> void {
    wgpu::TextureDescriptor texDesc{};
    texDesc.usage = wgpu::TextureUsage::RenderAttachment;
    texDesc.sampleCount = 4;
    texDesc.format = _textureFormat;
    texDesc.size.width = _width;
    texDesc.size.height = _height;
    texDesc.size.depthOrArrayLayers = 1;
    texDesc.label = "RendererShape2D Render Target";
    _tex = device.CreateTexture(&texDesc);
  }

  auto initShaderModule(const wgpu::Device& device) -> void {
    wgpu::ShaderSourceWGSL wgsl;
    wgsl.code = shader;
    wgpu::ShaderModuleDescriptor desc{};
    desc.nextInChain = &wgsl;
    desc.label = "RendererShape2D Shader";
    _shaderModule = device.CreateShaderModule(&desc);
  }

  auto initBufferLayout() -> void {
    // 2D Position
    _bufferLayout[0].arrayStride = 2 * sizeof(float);
    _bufferLayout[0].attributeCount = 1;
    _vertexAttribs[0].format = wgpu::VertexFormat::Float32x2;
    _bufferLayout[0].attributes = &_vertexAttribs[0];
  }

  auto initBindGroups(const wgpu::Device& device) -> void {
    // 1. Define Layout Entries
    std::array<wgpu::BindGroupLayoutEntry, 2> layoutEntries;
    // Transform Matrix entry
    layoutEntries[0].binding = 0;
    layoutEntries[0].visibility = wgpu::ShaderStage::Vertex;
    layoutEntries[0].buffer.type = wgpu::BufferBindingType::Uniform;
    layoutEntries[0].buffer.minBindingSize = sizeof(float) * 16;
    // Color uniform entry
    layoutEntries[1].binding = 1;
    layoutEntries[1].visibility = wgpu::ShaderStage::Fragment;
    layoutEntries[1].buffer.type = wgpu::BufferBindingType::Uniform;
    layoutEntries[1].buffer.minBindingSize = sizeof(float) * 4;

    // 2. describe and create group layout
    wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc{};
    bindGroupLayoutDesc.entryCount = layoutEntries.size();
    bindGroupLayoutDesc.entries = layoutEntries.data();
    _bindGroupLayout = device.CreateBindGroupLayout(&bindGroupLayoutDesc);

    // 3. Create bind group entries
    std::array<wgpu::BindGroupEntry, 2> entries;
    entries[0].binding = 0;
    entries[0].buffer = _transformBuffer;
    entries[0].offset = 0;
    entries[0].size = sizeof(float) * 16;
    entries[1].binding = 1;
    entries[1].buffer = _colorBuffer;
    entries[1].offset = 0;
    entries[1].size = sizeof(float) * 4;

    // 4. Create bind group
    wgpu::BindGroupDescriptor bindGroupDesc{};
    bindGroupDesc.label = "RendererShape2D Bind Group";
    bindGroupDesc.layout = _bindGroupLayout;
    bindGroupDesc.entryCount = entries.size();
    bindGroupDesc.entries = entries.data();
    _bindGroup = device.CreateBindGroup(&bindGroupDesc);
  }
};

}  // namespace Dusk
