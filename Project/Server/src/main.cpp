#include <iostream>
#include <chrono>
#include <thread>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

#include "server.h"
#include "utils.h"

int main() {
  sf::RenderWindow window(sf::VideoMode(800, 600), "WINDOW");

  std::chrono::high_resolution_clock frame_clock;
  std::chrono::high_resolution_clock::time_point time1;
  std::chrono::high_resolution_clock::time_point time2;
  float target_frame_time = 16.6666f;
  float last_frame_time = 0.0f;

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
    server.update(last_frame_time);
    fps_counter.setString(std::to_string(last_frame_time).substr(0, 5));
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


    // Lock tickrate to 60fps
    time2 = frame_clock.now();
    last_frame_time = std::chrono::duration_cast<std::chrono::duration<float>>(time2 - time1).count();
    time1 = frame_clock.now();

    // Sleep only for half of the remaining time to the target,
    // because the SO may leave the thread to sleep for more
    // than you specified.
    if (last_frame_time < target_frame_time) {
      std::this_thread::sleep_for(std::chrono::duration<float, std::milli>((target_frame_time - last_frame_time) * 0.5f));
    }
    // Spinlock the rest of the frame time to get the precise target
    while (last_frame_time <= target_frame_time) {
      time2 = frame_clock.now();
      last_frame_time += std::chrono::duration_cast<std::chrono::duration<float>>(time2 - time1).count();
    }
  } // while (window.isOpen())

  return 0;
}