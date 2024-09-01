#include <Dusk/App.hpp>
#include <iostream>

class UserInput : public Dusk::App {
  void onMouseDragged(double mouseX, double mouseY) {
    drawer.circle().xy(mouseX, mouseY).radius(15);
    drawer.draw();
  }

  void onMousePressed(double mouseX, double mouseY, int button) {
    std::cout << "CLICKED" << std::endl;
  }

  void onMouseReleased(double mouseX, double mouseY, int button) {
    std::cout << "RELEASED" << std::endl;
  }

  void onKeyPressed(int key) {
    if (key == GLFW_KEY_C) {
      drawer.clear(0);
      drawer.draw();
    }
  }
};

int main() {
  UserInput app;
  app.run();
}