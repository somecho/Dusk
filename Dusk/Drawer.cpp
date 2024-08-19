#include <Dusk/Builder/Buffer.hpp>
#include <Dusk/Drawer.hpp>
#include <Dusk/Shader.hpp>

namespace Dusk {

Drawer::Drawer(wgpu::Device& device, wgpu::Surface& surface,
               wgpu::TextureFormat format)
    : device(device), surface(surface), format(format) {
  transformBuffer = Dusk::Builder::Buffer<float, wgpu::BufferUsage::Uniform>()
                        .data(glm::mat4(1.0))
                        .addUsage(wgpu::BufferUsage::CopyDst)
                        .build(device);

  const char* shaderSource = R"(
    @group(0) @binding(0) var<uniform> transformMat: mat4x4f;

    struct VertexInput {
        @location(0) pos: vec2f,
        @location(1) col: vec4f
    };

    struct VertexOutput {
        @builtin(position) pos: vec4f,
        @location(0) col: vec4f
    };

    @vertex
    fn vs_main(in: VertexInput) -> VertexOutput {
        var out: VertexOutput;
        out.pos =  transformMat * vec4f(in.pos,0.0,1.0);
        out.col = in.col;
        return out;
    }

    @fragment
    fn fs_main(in: VertexOutput) -> @location(0) vec4f {
        return vec4f(in.col);
    })";

  Dusk::Shader shader =
      Dusk::ShaderBuilder().source(shaderSource).build(device);

  wgpu::VertexAttribute positionAttrib;
  positionAttrib.shaderLocation = 0;
  positionAttrib.format = wgpu::VertexFormat::Float32x2;
  positionAttrib.offset = 0;

  wgpu::VertexAttribute colorAttrib;
  colorAttrib.shaderLocation = 1;
  colorAttrib.format = wgpu::VertexFormat::Float32x4;
  colorAttrib.offset = 0;

  wgpu::VertexBufferLayout vertexBufferLayouts[2];
  vertexBufferLayouts[0].attributeCount = 1;
  vertexBufferLayouts[0].attributes = &positionAttrib;
  vertexBufferLayouts[0].stepMode = wgpu::VertexStepMode::Vertex;
  vertexBufferLayouts[0].arrayStride = 2 * sizeof(float);
  vertexBufferLayouts[1].attributeCount = 1;
  vertexBufferLayouts[1].attributes = &colorAttrib;
  vertexBufferLayouts[1].stepMode = wgpu::VertexStepMode::Vertex;
  vertexBufferLayouts[1].arrayStride = 4 * sizeof(float);

  wgpu::RenderPipelineDescriptor pipelineDesc;
  pipelineDesc.vertex.module = shader.mod;
  pipelineDesc.vertex.bufferCount = 2;
  pipelineDesc.vertex.buffers = vertexBufferLayouts;
  pipelineDesc.vertex.entryPoint = "vs_main";

  // primitive state
  pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
  pipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
  pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;
  pipelineDesc.primitive.cullMode = wgpu::CullMode::None;

  wgpu::ColorTargetState colTarget;
  colTarget.format = format;
  wgpu::FragmentState frag;
  frag.module = shader.mod;
  frag.entryPoint = "fs_main";
  frag.targetCount = 1;
  frag.targets = &colTarget;
  pipelineDesc.fragment = &frag;

  pipelineDesc.multisample.count = 1;
  pipelineDesc.multisample.mask = ~0u;  // all bits on
  pipelineDesc.multisample.alphaToCoverageEnabled = false;

  wgpu::BindGroupLayoutEntry bindingLayout;
  bindingLayout.binding = 0;
  bindingLayout.visibility = wgpu::ShaderStage::Vertex;
  bindingLayout.buffer.type = wgpu::BufferBindingType::Uniform;
  bindingLayout.buffer.minBindingSize = sizeof(float) * 16;

  // BIND GROUP LAYOUT
  wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc{};
  bindGroupLayoutDesc.entryCount = 1;
  bindGroupLayoutDesc.entries = &bindingLayout;
  wgpu::BindGroupLayout bindGroupLayout =
      device.CreateBindGroupLayout(&bindGroupLayoutDesc);

  //
  wgpu::BindGroupEntry binding;
  binding.binding = 0;
  binding.buffer = transformBuffer;
  binding.offset = 0;
  binding.size = sizeof(float) * 16;

  wgpu::BindGroupDescriptor bindGroupDesc{};
  bindGroupDesc.layout = bindGroupLayout;
  bindGroupDesc.entryCount = bindGroupLayoutDesc.entryCount;
  bindGroupDesc.entries = &binding;
  bindGroup = device.CreateBindGroup(&bindGroupDesc);

  // PIPELINE LAYOUT
  wgpu::PipelineLayoutDescriptor layoutDesc{};
  layoutDesc.bindGroupLayoutCount = 1;
  layoutDesc.bindGroupLayouts = &bindGroupLayout;

  pipelineDesc.layout = device.CreatePipelineLayout(&layoutDesc);
  pipeline = device.CreateRenderPipeline(&pipelineDesc);
}

Drawer::~Drawer() {
  pipeline = nullptr;
  if (vertexBuffer) {
    vertexBuffer.Destroy();
  }
  if (indexBuffer) {
    indexBuffer.Destroy();
  }
  if (colorBuffer) {
    colorBuffer.Destroy();
  }
}

Drawer& Drawer::clear(float r, float g, float b, float a) {
  return clear({r, g, b, a});
};

Drawer& Drawer::clear(float value, float alpha) {
  return clear({value, value, value, alpha});
};

Drawer& Drawer::clear(Rgba color) {
  m_clearColor = color;
  return *this;
}

Drawer& Drawer::color(float r, float g, float b, float a) {
  return color({r, g, b, a});
};

Drawer& Drawer::color(float value, float alpha) {
  return color({value, value, value, alpha});
};

Drawer& Drawer::color(Rgba color) {
  m_color = color;
  return *this;
};

Drawer& Drawer::rect(glm::vec2 pos, float w, float h) {
  return rect(pos.x, pos.y, w, h);
}

Drawer& Drawer::rect(float x, float y, float w, float h) {
  uint32_t startIndex = vertices.size() / 2;

  // vertex 0
  vertices.push_back(x);
  vertices.push_back(y);
  colors.push_back(m_color.r);
  colors.push_back(m_color.g);
  colors.push_back(m_color.b);
  colors.push_back(m_color.a);
  // vertex 1
  vertices.push_back(x + w);
  vertices.push_back(y);
  colors.push_back(m_color.r);
  colors.push_back(m_color.g);
  colors.push_back(m_color.b);
  colors.push_back(m_color.a);
  // vertex 2
  vertices.push_back(x + w);
  vertices.push_back(y + h);
  colors.push_back(m_color.r);
  colors.push_back(m_color.g);
  colors.push_back(m_color.b);
  colors.push_back(m_color.a);
  // vertex 3
  vertices.push_back(x);
  vertices.push_back(y + h);
  colors.push_back(m_color.r);
  colors.push_back(m_color.g);
  colors.push_back(m_color.b);
  colors.push_back(m_color.a);

  // tri 1
  indices.push_back(startIndex);
  indices.push_back(startIndex + 1);
  indices.push_back(startIndex + 2);
  // tri 2
  indices.push_back(startIndex);
  indices.push_back(startIndex + 2);
  indices.push_back(startIndex + 3);

  return *this;
}

void Drawer::draw() {
  wgpu::Queue queue = device.GetQueue();
  syncBuffer<float, wgpu::BufferUsage::Vertex>(vertexBuffer, vertices);
  syncBuffer<float, wgpu::BufferUsage::Vertex>(colorBuffer, colors);
  syncBuffer<uint32_t, wgpu::BufferUsage::Index>(indexBuffer, indices);

  wgpu::SurfaceTexture surfaceTexture;
  surface.GetCurrentTexture(&surfaceTexture);

  wgpu::TextureView view = surfaceTexture.texture.CreateView();

  // create encoder
  wgpu::CommandEncoder encoder = device.CreateCommandEncoder();
  // create attachment
  wgpu::RenderPassColorAttachment attachment{};
  attachment.view = view;
  attachment.loadOp = wgpu::LoadOp::Clear;
  attachment.storeOp = wgpu::StoreOp::Store;
  attachment.clearValue = wgpu::Color{m_clearColor.r, m_clearColor.g,
                                      m_clearColor.b, m_clearColor.a};

  // create render pass
  wgpu::RenderPassDescriptor renderDesc{};
  renderDesc.colorAttachmentCount = 1;
  renderDesc.colorAttachments = &attachment;
  wgpu::RenderPassEncoder renderPass = encoder.BeginRenderPass(&renderDesc);
  renderPass.SetPipeline(pipeline);
  renderPass.SetBindGroup(0, bindGroup);
  renderPass.SetVertexBuffer(0, vertexBuffer, 0, vertexBuffer.GetSize());
  renderPass.SetVertexBuffer(1, colorBuffer, 0, colorBuffer.GetSize());
  renderPass.SetIndexBuffer(indexBuffer, wgpu::IndexFormat::Uint32, 0,
                            indexBuffer.GetSize());
  renderPass.DrawIndexed((uint32_t)indices.size(), 1, 0, 0);
  renderPass.End();
  wgpu::CommandBuffer commands = encoder.Finish();
  queue.Submit(1, &commands);
  flushData();
};

void Drawer::setTransformMatrix(glm::mat4 mat) {
  std::vector<float> data;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      data.push_back(mat[i][j]);
    }
  }
  wgpu::Queue queue = device.GetQueue();
  queue.WriteBuffer(transformBuffer, 0, data.data(), sizeof(float) * 16);
}

void Drawer::flushData() {
  vertices.clear();
  indices.clear();
  colors.clear();
}

}  // namespace Dusk