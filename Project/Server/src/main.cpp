#include <iostream>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

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

  sf::Text state;
  state.setFont(font);
  state.setCharacterSize(40);
  state.setPosition(340, 270);

  std::string state_string;

  while (window.isOpen()) {
    // Input
    sf::Event e;
    if (window.pollEvent(e)) {
      if (e.type == sf::Event::Closed) {
        window.close();
        break;
      }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
      window.close();
      break;
    }

    // Update
    server.update();
    fps_counter.setString(std::to_string(server.lastFrameTime()).substr(0, 5));
    switch (server.getState()) {
      case Server::State::Starting: {
        state_string = "STARTING";
        
        break;
      }
      case Server::State::BuyTime: {
        state_string = "BUY TIME";

        break;
      }
      case Server::State::Game: {
        state_string = "GAME";

        break;
      }
      case Server::State::Error: {
        state_string = "ERROR";

        break;
      }
    }
    state.setString(state_string);

    // Draw
    window.clear();

    window.draw(fps_counter);
    window.draw(state);
    
    window.display();
  }

  return 0;
}