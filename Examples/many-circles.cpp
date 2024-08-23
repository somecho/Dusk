#include <Dusk/App.hpp>
#include <Dusk/Drawables.hpp>

class ManyCircles : public Dusk::App {
  void draw() {
    drawer.clear(0);

    int res = 500;
    float t = static_cast<float>(glfwGetTime());
    glm::vec2 start(getWidth() * 0.2, getHeight() * 0.5);
    glm::vec2 end(getWidth() * 0.8, getHeight() * 0.5);

    for (int i = 0; i <= res; i++) {
      float id = static_cast<float>(i) / static_cast<float>(res);
      auto pos = glm::mix(start, end, id);
      float radius = (sinf(t + id * 5.0) * 0.5 + 0.5) * 100 + 10;
      float r = sinf(t * 1.2 + id * 3.0) * 0.5 + 0.5;
      float g = sinf(t * 0.5 + id * 4.0 + M_PI) * 0.5 + 0.5;
      float b = sinf(t * 0.7 + id * 2.0 + M_PI * 1.5) * 0.5 + 0.5;
      drawer.circle().xy(pos).rgba(r, g, b).radius(radius);
    }

    drawer.draw();
  }
};

int main() {
  ManyCircles app;
  app.run();
}