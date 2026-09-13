export module Dusk.geom:Shape;

import std;

namespace Dusk::geom {

/**
 * Represents a shape primitive. `vertices` can be 2D or 3D depending
 * on the generator functions used. `color` applies to the entire
 * geometry.
 */
export typedef struct Shape {
  std::vector<float> vertices = {};
  std::array<float, 4> color = {0};
  std::vector<std::uint32_t> indices = {};
} Shape;

}  // namespace Dusk::geom
