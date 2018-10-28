#include <iostream>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

#include "button.h"
#include "client.h"
#include "utils.h"

int main() {
  sf::RenderWindow window(sf::VideoMode(800, 600), sf::String("WINDOW"));

  Client client;

  sf::Text fps_counter;
  sf::Font font;
  font.loadFromFile(GetResourcePath("fonts/arial.ttf"));
  fps_counter.setFont(font);
  fps_counter.setCharacterSize(16);
  fps_counter.setPosition(750, 10);

  sf::Text state;
  state.setFont(font);
  state.setCharacterSize(40);
  state.setPosition(340, 270);

  std::string state_string;

  Button button("+", sf::Vector2f(50.0f, 50.0f), 
    sf::Color::White, sf::Vector2f(100.0f, 100.0f), 14);

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
    client.update();
    fps_counter.setString(std::to_string(client.lastFrameTime()).substr(0, 5));
    switch (client.getState()) {
      case Client::State::Starting: {
        state_string = "STARTING";

        break;
      }
      case Client::State::BuyTime: {
        state_string = "BUY TIME";

        break;
      }
      case Client::State::Game: {
        state_string = "GAME";

        break;
      }
    }
    state.setString(state_string);

    // Draw
    window.clear(sf::Color(24, 128, 255, 255));

    window.draw(fps_counter);
    window.draw(state);
    button.draw(&window);

    window.display();
  }

  return 0;
}