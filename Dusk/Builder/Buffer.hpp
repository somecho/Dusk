#pragma once

#include <webgpu/webgpu_cpp.h>

#include <cstdint>
#include <vector>

namespace Dusk {
namespace Builder {

template <typename T, wgpu::BufferUsage U>
class Buffer {
 public:
  Buffer() {
    desc.usage = U;
  }

  Buffer<T, U>& data(std::vector<T> data) {
    m_data = data;
    desc.size = data.size() * sizeof(T);
    return *this;
  }

  Buffer<T, U>& addUsage(wgpu::BufferUsage usage) {
    desc.usage = desc.usage | usage;
    return *this;
  }

  Buffer<T, U>& usage(wgpu::BufferUsage usage) {
    desc.usage = usage;
    return *this;
  }

  Buffer<T, U>& mappedAtCreation(bool mappedAtCreation) {
    desc.mappedAtCreation = mappedAtCreation;
    return *this;
  }

  Buffer<T, U>& offset(uint64_t offset) {
    m_offset = offset;
    return *this;
  }

  Buffer<T, U>& label(const char* label) {
    desc.label = label;
    return *this;
  }

  wgpu::Buffer build(const wgpu::Device device) {
    wgpu::Queue queue = device.GetQueue();
    wgpu::Buffer buf = device.CreateBuffer(&desc);
    queue.WriteBuffer(buf, m_offset, m_data.data(), desc.size);
    return buf;
  }

 private:
  wgpu::BufferDescriptor desc;
  std::vector<T> m_data;
  uint64_t m_offset = 0;
};

}  // namespace Builder
}  // namespace Dusk