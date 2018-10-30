#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Network/UdpSocket.hpp>

#include "button.h"

class Client {
public:

  enum class State {
    Starting = 0,
    BuyTime,
    Game,
  };

  Client();
  ~Client();

  void setWindowRef(sf::RenderWindow* window);

  void update();
  void draw();
  State getState() const;

private:
  void startingState();
  void buyTimeState();
  void gameState();

  sf::UdpSocket m_socket;
  sf::Font m_font;
  State m_state;
  Button* m_plus_button = nullptr;
  Button* m_minus_button = nullptr;
  sf::RenderWindow* m_window_ref = nullptr;
  sf::Text* m_cards_bought_text = nullptr;
  sf::Text* m_buy_cards_text = nullptr;
  sf::Text* m_current_buying_time = nullptr;
  uint8_t m_cards_bought = 0;
  uint8_t m_connection_state = 0;
  bool m_connected = false;
  bool m_plus_button_was_pressed = false;
  bool m_minus_button_was_pressed = false;
};

#endif  // __CLIENT_H__