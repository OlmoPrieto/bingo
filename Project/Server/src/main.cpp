#include <iostream>
#include <SFML/Graphics/RenderWindow.hpp>

int main() {
  sf::RenderWindow window(sf::VideoMode(800, 600), "WINDOW");

  while (window.isOpen()) {
    window.clear();

    window.display();
  }

  window.close();

  return 0;
}