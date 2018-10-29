#include "client.h"

#include "utils.h"

Client::Client() {
  m_state = State::Starting;
  m_socket.bind(14195);
  m_socket.setBlocking(false);

  m_plus_button = new Button("+", sf::Vector2f(35.0f, 35.0f), 
    sf::Color::White, sf::Vector2f(485.0f, 250.0f), 18);
  m_minus_button = new Button("-", sf::Vector2f(35.0f, 35.0f), 
    sf::Color::White, sf::Vector2f(485.0f, 295.0f), 18);

  m_font.loadFromFile(GetResourcePath("fonts/arial.ttf"));
  m_cards_bought_text = new sf::Text();
  m_cards_bought_text->setFont(m_font);
  m_cards_bought_text->setCharacterSize(60);
  m_cards_bought_text->setPosition(385.0f, 235.0f);
  m_cards_bought_text->setString(std::to_string(m_cards_bought));

  m_buy_cards_text = new sf::Text();
  m_buy_cards_text->setFont(m_font);
  m_buy_cards_text->setCharacterSize(35);
  m_buy_cards_text->setPosition(200.0f, 165.0f);
  m_buy_cards_text->setString("How many cards do you want?");
}

Client::~Client() {
  delete m_plus_button;
  delete m_minus_button;
  delete m_cards_bought_text;
  delete m_buy_cards_text;
}

void Client::setWindowRef(sf::RenderWindow* window) {
  m_window_ref = window;
}

void Client::startingState() {
  if (m_connected == false) {
    uint64_t bytes_received = 0;
    uint16_t client_port = 14195;
    uint16_t server_port = 14194;
    uint8_t buffer[1024]; // 80 bytes should do, though
    sf::IpAddress ip_address = sf::IpAddress::LocalHost;
    memset(buffer, 0, 1024);
    Message msg((uint64_t)Message::MsgType::ConnectionRequest,
      m_connection_state + 1, nullptr);
    msg.fillBuffer(buffer, 1024);
    m_socket.send(buffer, (uint64_t)1024, 
      ip_address, server_port);

    memset(buffer, 0, 1024);
    if (m_socket.receive(buffer, (uint64_t)1024, bytes_received,
      ip_address, server_port) == sf::Socket::Status::Done) {
      uint64_t header = (uint64_t)buffer[0];
      if ((Message::MsgType)header == Message::MsgType::ConnectionRequest) {
        uint64_t data_received = (uint64_t)buffer[sizeof(uint64_t)];
        if (data_received > m_connection_state) {
          m_connection_state = data_received;
        }
        printf("Received state: %lu\n", m_connection_state);
      }
    }
  }

  if (m_connection_state >= 3) {
    m_connected = true;
    m_state = State::BuyTime;
  }
}

void Client::buyTimeState() {
  m_plus_button->update(m_window_ref);
  m_minus_button->update(m_window_ref);

  bool plus_button_pressed = m_plus_button->isPressed();
  bool minus_button_pressed = m_minus_button->isPressed();

  if (plus_button_pressed && m_plus_button_was_pressed == false) {
    if (m_cards_bought < 4) {
      ++m_cards_bought;
    }
  }
  else if (minus_button_pressed && m_minus_button_was_pressed == false) {
    if (m_cards_bought > 0) {
      --m_cards_bought;
    }
  }

  m_cards_bought_text->setString(std::to_string(m_cards_bought));

  m_plus_button_was_pressed = plus_button_pressed;
  m_minus_button_was_pressed = minus_button_pressed;
}

void Client::gameState() {

}

void Client::update() {
  switch (m_state) {
    case State::Starting: {
      startingState();

      break;
    }

    case State::BuyTime: {
      buyTimeState();

      break;
    }

    case State::Game: {
      break;
    }
  }
}

void Client::draw() {
  if (m_state == State::BuyTime) {
    m_plus_button->draw(m_window_ref);
    m_minus_button->draw(m_window_ref);
  
    m_window_ref->draw(*m_cards_bought_text);
    m_window_ref->draw(*m_buy_cards_text);
  }
}

Client::State Client::getState() const {
  return m_state;
}