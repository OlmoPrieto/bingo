#include <iostream>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "server.h"

int main() {
  sf::RenderWindow window(sf::VideoMode(800, 600), "WINDOW");

  Server server;

  while (window.isOpen()) {
    // Input
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
      window.close();
      break;
    }

    // Update
    server.update();

    // Draw
    window.clear();

    window.display();
  }

  return 0;
}