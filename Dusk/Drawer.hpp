#pragma once

#include <webgpu/webgpu_cpp.h>

#include <Dusk/Builder/Buffer.hpp>
#include <cstdint>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <vector>

namespace Dusk {

struct Rgba {
  float r;
  float g;
  float b;
  float a;
};

class Drawer {
 public:
  Drawer() = default;
  ~Drawer();
  Drawer(wgpu::Device& device, wgpu::Surface& surface,
         wgpu::TextureFormat format);

  Drawer& clear(float r, float g, float b, float a = 1.0);
  Drawer& clear(float value, float alpha = 1.0);
  Drawer& clear(Rgba color);

  Drawer& color(float r, float g, float b, float a = 1.0);
  Drawer& color(float value, float alpha = 1.0);
  Drawer& color(Rgba color);

  Drawer& rect(float x, float y, float w, float h);
  Drawer& rect(glm::vec2 pos, float w, float h);

  void draw();
  void setTransformMatrix(glm::mat4 mat);

 private:
  void flushData();

  template <typename T, wgpu::BufferUsage U>
  void syncBuffer(wgpu::Buffer& buffer, const std::vector<T>& data) {
    wgpu::Queue queue = device.GetQueue();
    if (!buffer) {
      buffer = Dusk::Builder::Buffer<T, U>()
                   .data(data)
                   .addUsage(wgpu::BufferUsage::CopyDst)
                   .build(device);
    } else if (buffer.GetSize() == sizeof(T) * data.size()) {
      queue.WriteBuffer(buffer, 0, data.data(), sizeof(T) * data.size());
    } else {
      buffer.Destroy();
      buffer = Dusk::Builder::Buffer<T, U>()
                   .data(data)
                   .addUsage(wgpu::BufferUsage::CopyDst)
                   .build(device);
    }
  };

  wgpu::Device device;
  wgpu::Surface surface;
  wgpu::RenderPipeline pipeline;
  wgpu::TextureFormat format;

  std::vector<float> vertices;
  std::vector<float> colors;
  std::vector<uint32_t> indices;

  wgpu::Buffer vertexBuffer;
  wgpu::Buffer colorBuffer;
  wgpu::Buffer indexBuffer;
  wgpu::Buffer transformBuffer;
  wgpu::BindGroup bindGroup;

  Rgba m_color = {1.0, 1.0, 1.0, 1.0};
  Rgba m_clearColor = {0.0, 0.0, 0.0, 0.0};
};

}  // namespace Dusk