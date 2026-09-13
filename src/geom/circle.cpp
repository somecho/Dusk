export module Dusk.geom:circle;

import std;

namespace Dusk::geom {

static constexpr std::uint16_t CIRCLE_RESOLUTION = 72;
static std::vector<float> g_sine_table;
static std::vector<float> g_cosine_table;

auto populate_tables(std::size_t num_vals) -> void {
  if (g_sine_table.size() != num_vals) {
    g_sine_table.resize(num_vals);
    for (std::size_t i = 0; i < num_vals; i++) {
      auto theta = (static_cast<float>(i) / static_cast<float>(num_vals)) *
                   (std::numbers::pi_v<float> * 2.f);
      g_sine_table.at(i) = std::sinf(theta);
    }
  }
  if (g_cosine_table.size() != num_vals) {
    g_cosine_table.resize(num_vals);
    for (std::size_t i = 0; i < num_vals; i++) {
      auto theta = (static_cast<float>(i) / static_cast<float>(num_vals)) *
                   (std::numbers::pi_v<float> * 2.f);
      g_cosine_table.at(i) = std::cosf(theta);
    }
  }
}

}  // namespace Dusk::geom

namespace Dusk::geom::circle {

export typedef struct Desc {
  float x = 0.f;
  float y = 0.f;
  float r = 1.f;
  std::uint16_t resolution = CIRCLE_RESOLUTION;
} Desc;

/**
 * @returns A 1D vector containing interleaved 2D coordinates of a
 * circle. The size of this vector is `(resolution + 1) * 2`. The
 * first 2D coordinate is the origin. To be used with indices.
 *
 * @param x x-coordinate of circle origin
 * @param y y-coordinate of circle origin
 * @param r radius of circle
 * @param resolution number of points to approximate the circle with
 */
export auto indexed_vertices(float x = 0.f, float y = 0.f, float r = 1.f,
                             std::uint16_t resolution = CIRCLE_RESOLUTION)
    -> std::vector<float> {
  populate_tables(static_cast<std::size_t>(resolution));
  std::vector<float> vtx;
  vtx.reserve(static_cast<std::size_t>(resolution + 1) * 2);
  vtx.emplace_back(x);
  vtx.emplace_back(y);
  for (std::uint16_t i = 0; i < resolution; i++) {
    float vx = g_cosine_table.at(i) * r + x;
    float vy = g_sine_table.at(i) * r + y;
    vtx.emplace_back(vx);
    vtx.emplace_back(vy);
  }
  return vtx;
}

export auto indexed_vertices(const Desc& desc = {}) -> std::vector<float> {
  return indexed_vertices(desc.x, desc.y, desc.r, desc.resolution);
}

export auto indices(std::uint16_t resolution = CIRCLE_RESOLUTION)
    -> std::vector<std::uint32_t> {
  static std::vector<std::uint32_t> indices;
  if (indices.size() != resolution * 3) {
    indices.resize(resolution * 3);
  }
  for (std::size_t i = 0; i < resolution; i++) {
    indices.at(i * 3) = 0;
    indices.at(i * 3 + 1) = (i % resolution) + 1;
    indices.at(i * 3 + 2) = ((i + 1) % resolution) + 1;
  }
  return indices;
}

}  // namespace Dusk::geom::circle
