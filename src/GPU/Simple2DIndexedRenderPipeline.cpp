module;

#include <webgpu/webgpu_cpp.h>

#include <glm/ext.hpp>

export module Dusk.GPU:Simple2DIndexedRenderPipeline;

import std;
import :RenderPipelineBuilder;

namespace Dusk::GPU {
export class Simple2DIndexedRenderPipeline {
  wgpu::ShaderModule shaderModule;
  wgpu::RenderPipeline pipeline;
  std::array<wgpu::VertexBufferLayout, 2> bufferLayout = {};
  std::array<wgpu::VertexAttribute, 2> vertexAttribs = {};

  auto createShaderModule(const wgpu::Device& device) -> void {
    wgpu::ShaderSourceWGSL wgsl;
    wgsl.code = shader;
    wgpu::ShaderModuleDescriptor shaderDesc{};
    shaderDesc.nextInChain = &wgsl;
    shaderModule = device.CreateShaderModule(&shaderDesc);
  }

  auto initBufferLayout() -> void {
    // Position attribute
    bufferLayout[0].arrayStride = 2 * sizeof(float);
    bufferLayout[0].attributeCount = 1;
    vertexAttribs[0].format = wgpu::VertexFormat::Float32x2;
    bufferLayout[0].attributes = &vertexAttribs[0];

    // Color attribute
    bufferLayout[1].arrayStride = 3 * sizeof(float);
    bufferLayout[1].attributeCount = 1;
    vertexAttribs[1].format = wgpu::VertexFormat::Float32x3;
    vertexAttribs[1].shaderLocation = 1;
    bufferLayout[1].attributes = &vertexAttribs[1];
  }

  wgpu::BindGroupLayout bindGroupLayout;

 public:
  Simple2DIndexedRenderPipeline() {};
  Simple2DIndexedRenderPipeline(const wgpu::Device& device,
                                const wgpu::Surface& surface) {
    createShaderModule(device);
    initBufferLayout();
    wgpu::SurfaceTexture tex;
    surface.GetCurrentTexture(&tex);

    wgpu::BindGroupLayoutEntry bindingLayout;
    bindingLayout.binding = 0;
    bindingLayout.visibility = wgpu::ShaderStage::Vertex;
    bindingLayout.buffer.type = wgpu::BufferBindingType::Uniform;
    bindingLayout.buffer.minBindingSize = sizeof(float) * 16;

    wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc{};
    bindGroupLayoutDesc.entryCount = 1;
    bindGroupLayoutDesc.entries = &bindingLayout;
    bindGroupLayout = device.CreateBindGroupLayout(&bindGroupLayoutDesc);

    pipeline = Dusk::GPU::RenderPipelineBuilder()
                   .vertexBuffers(2)
                   .vertexShaderModule(shaderModule)
                   .bufferLayouts(bufferLayout.data())
                   .colorTargets(1)
                   .colorTargetFormat(0, tex.texture.GetFormat())
                   .fragmentShaderModule(shaderModule)
                   .bindGroupLayouts(&bindGroupLayout)
                   .build(device);
  }

  auto render(const wgpu::Device& device, wgpu::Surface& surface,
              const std::vector<float>& vertices,
              const std::vector<float>& colors,
              const std::vector<uint32_t>& indices) {
    wgpu::SurfaceTexture tex;
    surface.GetCurrentTexture(&tex);
    auto view = tex.texture.CreateView();

    wgpu::TextureDescriptor texDesc{};
    texDesc.usage = wgpu::TextureUsage::RenderAttachment;
    texDesc.sampleCount = 4;
    texDesc.format = tex.texture.GetFormat();
    texDesc.size.width = tex.texture.GetWidth();
    texDesc.size.height = tex.texture.GetHeight();
    texDesc.size.depthOrArrayLayers = 1;
    auto t = device.CreateTexture(&texDesc);

    auto ortho = glm::ortho<float>(0, tex.texture.GetWidth(),
                                   tex.texture.GetHeight(), 0, -1, 1);

    std::array<wgpu::BufferDescriptor, 4> bufDesc;
    bufDesc[0].usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst;
    bufDesc[0].size = sizeof(vertices[0]) * vertices.size();
    auto vertexBuffer = device.CreateBuffer(&bufDesc[0]);
    device.GetQueue().WriteBuffer(vertexBuffer, 0, vertices.data(),
                                  bufDesc[0].size);

    bufDesc[1].usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst;
    bufDesc[1].size = sizeof(colors[1]) * colors.size();
    auto colorBuffer = device.CreateBuffer(&bufDesc[1]);
    device.GetQueue().WriteBuffer(colorBuffer, 0, colors.data(),
                                  bufDesc[1].size);

    bufDesc[2].usage = wgpu::BufferUsage::Index | wgpu::BufferUsage::CopyDst;
    bufDesc[2].size = sizeof(indices[1]) * indices.size();
    auto indexBuffer = device.CreateBuffer(&bufDesc[2]);
    device.GetQueue().WriteBuffer(indexBuffer, 0, indices.data(),
                                  bufDesc[2].size);

    bufDesc[3].usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
    bufDesc[3].size = sizeof(float) * 16;
    auto transformBuffer = device.CreateBuffer(&bufDesc[3]);
    device.GetQueue().WriteBuffer(transformBuffer, 0, glm::value_ptr(ortho),
                                  bufDesc[3].size);

    wgpu::RenderPassColorAttachment attachment;
    attachment.loadOp = wgpu::LoadOp::Load;
    attachment.storeOp = wgpu::StoreOp::Store;
    attachment.clearValue = {0.0, 0.0, 0.0, 0.0};
    attachment.view = t.CreateView();
    attachment.resolveTarget = view;

    wgpu::RenderPassDescriptor renderPass{};
    renderPass.colorAttachmentCount = 1;
    renderPass.colorAttachments = &attachment;

    wgpu::BindGroupEntry binding;
    binding.binding = 0;
    binding.buffer = transformBuffer;
    binding.offset = 0;
    binding.size = sizeof(float) * 16;

    wgpu::BindGroupDescriptor bindGroupDesc{};
    bindGroupDesc.layout = bindGroupLayout;
    bindGroupDesc.entryCount = 1;
    bindGroupDesc.entries = &binding;

    auto bindGroup = device.CreateBindGroup(&bindGroupDesc);

    auto encoder = device.CreateCommandEncoder();
    auto pass = encoder.BeginRenderPass(&renderPass);
    pass.SetPipeline(pipeline);
    pass.SetBindGroup(0, bindGroup);
    pass.SetVertexBuffer(0, vertexBuffer);
    pass.SetVertexBuffer(1, colorBuffer);
    pass.SetIndexBuffer(indexBuffer, wgpu::IndexFormat::Uint32);
    pass.DrawIndexed(indices.size());
    pass.End();
    auto commands = encoder.Finish();
    device.GetQueue().Submit(1, &commands);
    vertexBuffer.Destroy();
    colorBuffer.Destroy();
    indexBuffer.Destroy();
    transformBuffer.Destroy();
    t.Destroy();
  }

  static constexpr std::string_view shader = R"(
        @group(0) @binding(0) var<uniform> transformMat: mat4x4f;
        struct VertexOutput {
          @builtin(position) position: vec4f,
          @location(0) color: vec3f
        };
        @vertex fn vs(@location(0) pos : vec2f, @location(1) col: vec3f) -> VertexOutput {
            var out: VertexOutput;
            out.position = transformMat * vec4(pos,0.0, 1.0);
            out.color = col;
            return out;
        }

        @fragment fn fs(in: VertexOutput) -> @location(0) vec4f {
            return vec4f(in.color, 1);
        }
    )";
};

}  // namespace Dusk::GPU
