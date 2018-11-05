#include "client.h"

#include "utils.h"

Client::Client() {
  m_state = State::Starting;
  m_socket.bind(14195);
  m_socket.setBlocking(false);

  m_font.loadFromFile(GetResourcePath("fonts/arial.ttf"));

  m_cards_bought_text.setFont(m_font);
  m_cards_bought_text.setCharacterSize(60);
  m_cards_bought_text.setPosition(385.0f, 235.0f);
  m_cards_bought_text.setString(std::to_string(m_cards_bought));

  m_buy_cards_text.setFont(m_font);
  m_buy_cards_text.setCharacterSize(35);
  m_buy_cards_text.setPosition(200.0f, 165.0f);
  m_buy_cards_text.setString("How many cards do you want?");

  m_current_buying_time.setFont(m_font);
  m_current_buying_time.setCharacterSize(35);
  m_current_buying_time.setPosition(550.0f, 55.0f);
  m_current_buying_time.setString("0");
}

Client::~Client() {

}

void Client::setWindowRef(sf::RenderWindow* window) {
  m_window_ref = window;
}

void Client::startingState() {
  if (m_connected == false) {
    uint64_t bytes_received = 0;
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
      }
    }
  }

  if (m_connection_state >= 3) {
    m_connected = true;
    m_state = State::BuyTime;
    m_display_buttons = true;
    m_connection_state = 0; // to reuse it later
  }
}

void Client::setBoughtCardsDistribution() {
  sf::Vector2f size;
  sf::Vector2f position;
  sf::Vector2u window_size = m_window_ref->getSize();
  switch (m_cards_bought) {
    case 1: {
      size.x = window_size.x * 0.5f;
      size.y = size.x * 0.5f;
      position.x = window_size.x * 0.5f - size.x * 0.5f;
      position.y = window_size.y * 0.5f;

      m_cards[0].create(size, position);

      break;
    }
    case 2: {
      size.x = window_size.x * 0.4f; 
      size.y = size.x * 0.5f;

      position.x = window_size.x * 0.25f - size.x * 0.5f;
      position.y = window_size.y * 0.5f;
      m_cards[0].create(size, position);

      position.x = window_size.x * 0.75f - size.x * 0.5f;
      m_cards[1].create(size, position);

      break;
    }
    case 3: {
      size.x = window_size.x * 0.35f; 
      size.y = size.x * 0.5f;
      
      position.x = window_size.x * 0.25f - size.x * 0.5f;
      position.y = window_size.y * 0.4f;
      m_cards[0].create(size, position);

      position.x = window_size.x * 0.75f - size.x * 0.5f;
      m_cards[1].create(size, position);

      position.x = window_size.x * 0.5f - size.x * 0.5f;
      position.y += size.y * 1.25f;
      m_cards[2].create(size, position);

      break;
    }
    case 4: {
      size.x = window_size.x * 0.3f;
      size.y = size.x * 0.5f;

      position.x = window_size.x * 0.25f - size.x * 0.5f;
      position.y = window_size.y * 0.4f;
      m_cards[0].create(size, position);

      position.x = window_size.x * 0.75f - size.x * 0.5f;
      m_cards[1].create(size, position);

      position.x = window_size.x * 0.25f - size.x * 0.5f;
      position.y += size.y * 1.25f;
      m_cards[2].create(size, position);

      position.x = window_size.x * 0.75f - size.x * 0.5f;
      m_cards[3].create(size, position);

      break;
    }
  }
}

void Client::buyTimeState() {
  // Manage remaining buying time received and calculated by the server
  uint64_t bytes_received = 0;
  uint16_t server_port = 14194;
  uint8_t buffer[1024]; // 80 bytes should do, though
  sf::IpAddress ip_address = sf::IpAddress::LocalHost;
  memset(buffer, 0, 1024);
  if (m_socket.receive(buffer, (uint64_t)1024, bytes_received,
    ip_address, server_port) == sf::Socket::Status::Done) {
    uint64_t header = (uint64_t)buffer[0];
    if ((Message::MsgType)header == Message::MsgType::CurrentBuyingTime) {
      uint64_t data_received = (uint64_t)buffer[sizeof(uint64_t)];
      if (data_received >= 0) {
        m_current_buying_time.setString(std::to_string(data_received));
      }
    }
  }

  // Handle buttons
  if (m_display_buttons == true) {
    m_plus_button.update(m_window_ref);
    m_minus_button.update(m_window_ref);
    m_confirm_purchase_button.update(m_window_ref);

    bool plus_button_pressed = m_plus_button.isPressed();
    bool minus_button_pressed = m_minus_button.isPressed();
    bool purchase_button_pressed = m_confirm_purchase_button.isPressed();

    if (plus_button_pressed && m_plus_button_was_pressed == false) {
      if (m_cards_bought < MAX_BUYABLE_CARDS) {
        ++m_cards_bought;
      }
    }
    else if (minus_button_pressed && m_minus_button_was_pressed == false) {
      if (m_cards_bought > 0) {
        --m_cards_bought;
      }
    }
    else if (purchase_button_pressed && m_confirm_purchase_button_was_pressed == false) {
      if (m_cards_bought > 0) {
        m_display_buttons = false;
        setBoughtCardsDistribution();
      }
    }

    m_cards_bought_text.setString(std::to_string(m_cards_bought));

    m_plus_button_was_pressed = plus_button_pressed;
    m_minus_button_was_pressed = minus_button_pressed;
    m_confirm_purchase_button_was_pressed = purchase_button_pressed;
  }
  else {  // If not displaying buttons, the bought cards are displayed

  }
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
    if (m_display_buttons == true) {
      m_plus_button.draw(m_window_ref);
      m_minus_button.draw(m_window_ref);
      m_confirm_purchase_button.draw(m_window_ref);
    
      m_window_ref->draw(m_cards_bought_text);
      m_window_ref->draw(m_buy_cards_text);
      m_window_ref->draw(m_current_buying_time);
    }
    else {
      for (uint8_t i = 0; i < m_cards_bought; ++i) {
        m_cards[i].draw(m_window_ref);
      }
    }
  }
}

Client::State Client::getState() const {
  return m_state;
}