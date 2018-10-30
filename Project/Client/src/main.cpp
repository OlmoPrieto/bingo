#include <iostream>
#include <chrono>
#include <thread>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

#include "card.h"
#include "client.h"
#include "utils.h"

int main() {
  sf::RenderWindow window(sf::VideoMode(800, 600), "");

  std::chrono::high_resolution_clock frame_clock;
  std::chrono::high_resolution_clock::time_point time1;
  std::chrono::high_resolution_clock::time_point time2;
  float target_frame_time = 16.6666f;
  float last_frame_time = 0.0f;

  Client client;
  client.setWindowRef(&window);

  sf::Text fps_counter;
  sf::Font font;
  font.loadFromFile(GetResourcePath("fonts/arial.ttf"));
  fps_counter.setFont(font);
  fps_counter.setCharacterSize(16);
  fps_counter.setPosition(750, 10);

  sf::Text state;
  state.setFont(font);
  state.setCharacterSize(40);
  state.setPosition(310, 50);

  std::string state_string;

  Card card(sf::Vector2f(200, 100), sf::Vector2f(200, 250));

  while (window.isOpen()) {
    time1 = frame_clock.now();

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
    fps_counter.setString(std::to_string(last_frame_time).substr(0, 5));
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
    window.clear(sf::Color(24, 164, 255, 255));

    window.draw(fps_counter);
    window.draw(state);
    client.draw();
    card.draw(&window);

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
  } // while(window.isOpen())

  return 0;
}