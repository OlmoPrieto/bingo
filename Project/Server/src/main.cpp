#include <iostream>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "server.h"

int main() {
  sf::RenderWindow window(sf::VideoMode(800, 600), "WINDOW");

  Server server;
  sf::Text fps_counter;
  sf::Font font;
  font.loadFromFile("../common/resources/fonts/arial.ttf");
  fps_counter.setFont(font);
  fps_counter.setCharacterSize(16);
  fps_counter.setPosition(750, 10);

  while (window.isOpen()) {
    // Input
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
      window.close();
      break;
    }

    // Update
    server.update();
    fps_counter.setString(std::to_string(server.lastFrameTime()).substr(0, 5));

    // Draw
    window.clear();

    window.draw(fps_counter);
    
    window.display();
  }

  return 0;
}