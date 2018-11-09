#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Network/UdpSocket.hpp>

#include "button.h"
#include "card.h"
#include "utils.h"

constexpr uint8_t MAX_BUYABLE_CARDS = 4;

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
  void connectionHandshake(Message::MsgType msg_type =
    Message::MsgType::Handshake); // Send and then receive
  void startingState();
  void buyTimeState();
  void gameState();
  void setBoughtCardsDistribution();

  sf::UdpSocket m_socket;
  sf::Font m_font;
  sf::Text m_cards_bought_text;
  sf::Text m_buy_cards_text;
  sf::Text m_current_buying_time;
  Button m_plus_button = { "+", sf::Vector2f(35.0f, 35.0f), 
    sf::Color::White, sf::Vector2f(485.0f, 250.0f), 18 };
  Button m_minus_button = { "-", sf::Vector2f(35.0f, 35.0f), 
    sf::Color::White, sf::Vector2f(485.0f, 295.0f), 18 };
  Button m_confirm_purchase_button = { "Purchase", sf::Vector2f(150, 35.0f), 
    sf::Color::White, sf::Vector2f(600.0f, 275.0f), 24 };
  State m_state;
  Card m_cards[MAX_BUYABLE_CARDS];
  sf::RenderWindow* m_window_ref = nullptr;
  uint8_t m_cards_bought = 0;
  uint8_t m_connection_state = 0;
  bool m_connected = false;
  bool m_display_buttons = false;
  bool m_plus_button_was_pressed = false;
  bool m_minus_button_was_pressed = false;
  bool m_confirm_purchase_button_was_pressed = false;
  bool m_bought_cards_sent = false;
  bool m_numbers_checked = false;
};

#endif  // __CLIENT_H__