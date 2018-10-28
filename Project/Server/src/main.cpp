#include <iostream>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

#include "server.h"
#include "utils.h"

int main() {
  sf::RenderWindow window(sf::VideoMode(800, 600), "WINDOW");

  Server server;
  
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

  sf::Text connected_players_text;
  connected_players_text.setFont(font);
  connected_players_text.setCharacterSize(16);
  connected_players_text.setPosition(15, 10);
  std::string connected_players_string = "Connected players: ";

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

    connected_players_text.setString(connected_players_string + 
      std::to_string(server.getConnectedPlayers()));

    // Draw
    window.clear();

    window.draw(fps_counter);
    window.draw(state);
    window.draw(connected_players_text);
    
    window.display();
  }

  return 0;
}