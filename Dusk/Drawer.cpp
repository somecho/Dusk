#include <Dusk/Drawer.hpp>
#include <Dusk/Shader.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/geometric.hpp>
#include <numbers>

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
        @location(0) pos: vec3f,
        @location(1) col: vec4f
    };

    struct VertexOutput {
        @builtin(position) pos: vec4f,
        @location(0) col: vec4f
    };

    @vertex
    fn vs_main(in: VertexInput) -> VertexOutput {
        var out: VertexOutput;
        out.pos =  transformMat * vec4f(in.pos,1.0);
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
  vertexBufferLayouts[0].arrayStride = 3 * sizeof(float);
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
  colTarget.format = this->format;
  wgpu::FragmentState frag;
  frag.module = shader.mod;
  frag.entryPoint = "fs_main";
  frag.targetCount = 1;
  frag.targets = &colTarget;
  pipelineDesc.fragment = &frag;

  wgpu::TextureDescriptor texDesc{};
  texDesc.usage = wgpu::TextureUsage::RenderAttachment;
  texDesc.sampleCount = 4;
  texDesc.format = this->format;
  wgpu::SurfaceTexture st;
  surface.GetCurrentTexture(&st);
  texDesc.size.width = st.texture.GetWidth();
  texDesc.size.height = st.texture.GetHeight();
  texDesc.size.depthOrArrayLayers = 1;

  tex = device.CreateTexture(&texDesc);

  pipelineDesc.multisample.count = 4;
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

void Drawer::clear(float r, float g, float b, float a) {
  clear({r, g, b, a});
};

void Drawer::clear(float value, float alpha) {
  clear({value, value, value, alpha});
};

void Drawer::clear(Rgba color) {
  m_clearColor = color;
}

Drawable::Rect& Drawer::rect() {
  return shape<Drawable::Rect>();
}

Drawable::Circle& Drawer::circle() {
  return shape<Drawable::Circle>();
}

Drawable::Ellipse& Drawer::ellipse() {
  return shape<Drawable::Ellipse>();
}

Drawable::Triangle& Drawer::tri() {
  return shape<Drawable::Triangle>();
}

Drawable::Line& Drawer::line() {
  return shape<Drawable::Line>();
}

void Drawer::draw() {
  uint32_t startIndex = 0;
  for (auto drawable : drawables) {
    if (std::holds_alternative<Drawable::Rect>(*drawable)) {
      processRect(std::get<Drawable::Rect>(*drawable), startIndex);
      startIndex += 4;
    } else if (std::holds_alternative<Drawable::Circle>(*drawable)) {
      auto c = std::get<Drawable::Circle>(*drawable);
      processCircle(c, startIndex);
      startIndex += c.res() + 1;
    } else if (std::holds_alternative<Drawable::Ellipse>(*drawable)) {
      auto e = std::get<Drawable::Ellipse>(*drawable);
      processEllipse(e, startIndex);
      startIndex += e.res() + 1;
    } else if (std::holds_alternative<Drawable::Triangle>(*drawable)) {
      processTriangle(std::get<Drawable::Triangle>(*drawable), startIndex);
      startIndex += 3;
    } else if (std::holds_alternative<Drawable::Line>(*drawable)) {
      processLine(std::get<Drawable::Line>(*drawable), startIndex);
      startIndex += 4;
    }
  }

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
  attachment.view = tex.CreateView();
  attachment.resolveTarget = view;
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
  drawables.clear();
}

void Drawer::processRect(Drawable::Rect& r, uint32_t startIndex) {
  // vertex 0
  vertices.push_back(r.x());
  vertices.push_back(r.y());
  vertices.push_back(r.z());
  // vertex 1
  vertices.push_back(r.x() + r.w());
  vertices.push_back(r.y());
  vertices.push_back(r.z());
  // vertex 2
  vertices.push_back(r.x() + r.w());
  vertices.push_back(r.y() + r.h());
  vertices.push_back(r.z());
  // vertex 3
  vertices.push_back(r.x());
  vertices.push_back(r.y() + r.h());
  vertices.push_back(r.z());

  // 4 vertices
  const std::vector<float> rgba = {r.r(), r.g(), r.b(), r.a()};
  for (int i = 0; i < 4; i++) {
    colors.insert(colors.end(), rgba.begin(), rgba.end());
  }

  indices.insert(indices.end(), {startIndex, startIndex + 1, startIndex + 2,
                                 startIndex, startIndex + 2, startIndex + 3});
}

void Drawer::processCircle(Drawable::Circle& c, uint32_t startIndex) {
  const float x = c.x();
  const float y = c.y();
  const float z = c.z();
  const float r = c.r();
  const float g = c.g();
  const float b = c.b();
  const float a = c.a();
  const float radius = c.radius();
  const uint32_t res = c.res();

  vertices.insert(vertices.end(), {x, y, z});
  for (uint32_t i = 0; i < res; i++) {
    float id = static_cast<float>(i) / static_cast<float>(res);
    float theta = id * std::numbers::pi_v<float> * 2.0;
    vertices.insert(vertices.end(),
                    {cosf(theta) * radius + x, sinf(theta) * radius + y, z});
  }

  for (uint32_t i = 0; i <= res; i++) {
    colors.insert(colors.end(), {r, g, b, a});
  }

  for (uint32_t i = 0; i < res; i++) {
    uint32_t i1 = startIndex;
    uint32_t i2 = startIndex + i + 1;
    uint32_t i3 = startIndex + (i + 1) % res + 1;
    indices.insert(indices.end(), {i1, i2, i3});
  }
}

void Drawer::processEllipse(Drawable::Ellipse& e, uint32_t startIndex) {
  const float x = e.x();
  const float y = e.y();
  const float z = e.z();
  const float r = e.r();
  const float g = e.g();
  const float b = e.b();
  const float a = e.a();
  const float w = e.w();
  const float h = e.h();
  const uint32_t res = e.res();

  vertices.insert(vertices.end(), {x, y, z});
  for (uint32_t i = 0; i < res; i++) {
    float id = static_cast<float>(i) / static_cast<float>(res);
    float theta = id * std::numbers::pi_v<float> * 2.0;
    vertices.insert(vertices.end(),
                    {cosf(theta) * w + x, sinf(theta) * h + y, z});
  }

  for (uint32_t i = 0; i <= res; i++) {
    colors.insert(colors.end(), {r, g, b, a});
  }

  for (uint32_t i = 0; i < res; i++) {
    uint32_t i1 = startIndex;
    uint32_t i2 = startIndex + i + 1;
    uint32_t i3 = startIndex + (i + 1) % res + 1;
    indices.insert(indices.end(), {i1, i2, i3});
  }
}

void Drawer::processTriangle(Drawable::Triangle& t, uint32_t startIndex) {
  const auto [x1, y1, z1] = t.p1<Triplet>();
  const auto [x2, y2, z2] = t.p2<Triplet>();
  const auto [x3, y3, z3] = t.p3<Triplet>();
  vertices.insert(vertices.end(), {x1, y1, z1});
  vertices.insert(vertices.end(), {x2, y2, z2});
  vertices.insert(vertices.end(), {x3, y3, z3});
  const std::vector<float> rgba = {t.r(), t.g(), t.b(), t.a()};
  for (int i = 0; i < 3; i++) {
    colors.insert(colors.end(), rgba.begin(), rgba.end());
  }
  indices.insert(indices.end(), {startIndex, startIndex + 1, startIndex + 2});
}

void Drawer::processLine(Drawable::Line& l, uint32_t startIndex) {
  const auto p1 = l.p1<glm::vec3>();
  const auto p2 = l.p2<glm::vec3>();
  glm::vec3 dir = glm::normalize(p2 - p1) * glm::vec3(l.thickness() * 0.5f);
  glm::vec3 bitan =
      glm::rotate(glm::mat4(1), std::numbers::pi_v<float> * 0.5f, {0, 0, 1}) *
      glm::vec4(dir, 1);

  const auto px1 = p1 + bitan;
  const auto px2 = p2 + bitan;
  const auto px3 = p2 - bitan;
  const auto px4 = p1 - bitan;
  vertices.insert(vertices.end(), {px1.x, px1.y, px1.z});
  vertices.insert(vertices.end(), {px2.x, px2.y, px2.z});
  vertices.insert(vertices.end(), {px3.x, px3.y, px3.z});
  vertices.insert(vertices.end(), {px4.x, px4.y, px4.z});

  const std::vector<float> rgba = {l.r(), l.g(), l.b(), l.a()};
  for (int i = 0; i < 4; i++) {
    colors.insert(colors.end(), rgba.begin(), rgba.end());
  }

  indices.insert(indices.end(), {startIndex, startIndex + 1, startIndex + 2,
                                 startIndex, startIndex + 2, startIndex + 3});
}

}  // namespace Dusk