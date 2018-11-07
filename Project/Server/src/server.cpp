#include "server.h"

#include <cmath>

Server::Server() {
  m_state = State::Starting;

  m_socket.bind(14194);
  m_socket.setBlocking(false);
}

Server::~Server() {

}

void Server::connectionHandshake(Message::MsgType msg_type) {
  uint64_t bytes_received = 0;
  uint16_t client_port = 14195;
  uint8_t buffer[1024]; // 80 bytes should do, though
  sf::IpAddress ip_address;
  memset(buffer, 0, 1024);
  if (m_socket.receive(buffer, (uint64_t)1024, bytes_received,
    ip_address, client_port) == sf::Socket::Status::Done) {
    
    int8_t player_id = getPlayerIDByIpAddress(ip_address.toString());
    if (player_id == -1) {
      player_id = 0;
      for (uint8_t i = 0; i < MAX_PLAYERS; ++i) {
        if (m_players_states[i].player_id != -1) {
          ++player_id;
        }
      }
      m_players_states[player_id].ip_address = ip_address;
      m_players_states[player_id].player_id = player_id;
      player_id = m_players_states[player_id].player_id;
    }

    // Check if the received packet contains the requested message
    uint64_t header = (uint64_t)buffer[0];
    if ((Message::MsgType)header == msg_type) {
      uint64_t data_received = (uint64_t)buffer[sizeof(uint64_t)];
      if (data_received > m_players_states[player_id].connection_state) {
        m_players_states[player_id].connection_state = data_received;
      }

      memset(buffer, 0, 1024);
      Message msg((uint64_t)msg_type,
        m_players_states[player_id].connection_state + 1, nullptr);
      msg.fillBuffer(buffer, 1024);
      m_socket.send(buffer, (uint64_t)1024,
        m_players_states[player_id].ip_address, client_port);
    }
  }
}

void Server::startingState() {
  if (getConnectedPlayers() < 1) {
    for (uint8_t i = 0; i < MAX_PLAYERS; ++i) {
      connectionHandshake(Message::MsgType::ConnectionRequest);
      if (m_players_states[i].connection_state >= 3) {
        m_players_states[i].connected = true;
        m_players_states[i].connection_state = 0; // to reuse it later
      }
    }
  }
  else {
    m_state = State::BuyTime;
  }
}

void Server::buyTimeState(float dt) {
  if (m_remaining_buy_time - dt >= 0.0f) {
    m_remaining_buy_time -= dt * 0.5f;
  }
  // TODO: else {} start over 30 seconds again if none of the players bought a card

  for (uint8_t i = 0; i < getConnectedPlayers(); ++i) {
    // Send time remaining information
    uint64_t bytes_received = 0;
    uint16_t client_port = 14195;
    uint8_t buffer[1024]; // 80 bytes should do, though
    sf::IpAddress ip_address;
    memset(buffer, 0, 1024);
    Message msg((uint64_t)Message::MsgType::CurrentBuyingTime,
      (uint64_t)(std::round(m_remaining_buy_time * 0.001f)), nullptr);
    msg.fillBuffer(buffer, 1024);
    m_socket.send(buffer, (uint64_t)1024,
      m_players_states[i].ip_address, client_port);
  
    // Check for bought cards messages && its handshake
    bytes_received = 0;
    memset(buffer, 0, 1024);
    if (m_socket.receive(buffer, (uint64_t)1024, bytes_received,
      ip_address, client_port) == sf::Socket::Status::Done) {

      int8_t player_id = getPlayerIDByIpAddress(ip_address.toString());

      if (m_players_states[player_id].cards_bought == 0) {
        // However, a player can buy zero cards and when the time runs out,
        // the game will continue (if the other player bought at least one).
        // So maybe the code will keep entering here for this particular player
        // but when the time runs out the code will continue.
      
        // Check if the received packet contains a card bought message
        uint64_t header = (uint64_t)buffer[0];
        if ((Message::MsgType)header == Message::MsgType::BoughtCards) {
          m_players_states[player_id].cards_bought = 
            (uint64_t)buffer[sizeof(uint64_t)];
          connectionHandshake(Message::MsgType::BoughtCardsConfirmation);
        }
      }
      else {
        if (m_players_states[player_id].connection_state > 0) {
          connectionHandshake(Message::MsgType::BoughtCardsConfirmation);
          if (m_players_states[player_id].connection_state >= 3) {
            // Player X cards bought confirmed
            printf("Player %d bought %u cards\n", 
              m_players_states[player_id].player_id,
              m_players_states[player_id].cards_bought);

            m_players_states[player_id].connection_state = 0;
          }
        }
      }
    }

    // Check for card numbers
    if (m_players_states[i].cards_bought > 0) {
      bytes_received = 0;
      memset(buffer, 0, 1024);
      if (m_socket.receive(buffer, (uint64_t)1024, bytes_received,
        ip_address, client_port) == sf::Socket::Status::Done) {

        // Check if the received packet contains a card numbers message
        uint64_t header = (uint64_t)buffer[0];
        //if (m_players_states[i].confirmed_cards )
        if ((Message::MsgType)header == Message::MsgType::CardNumbers) {
          uint64_t card_index = (uint64_t)buffer[sizeof(uint64_t)];
          Message msg((uint64_t)Message::MsgType::CardNumbers,
            card_index, buffer + sizeof(uint64_t) * 2);
          msg.getCardNumbers(&m_players_states[i].cards_numbers[card_index]);
        }
      }
    }
  }
}

void Server::update(float dt) {
  switch (m_state) {
    case State::Starting: {
      startingState();

      break;
    }

    case State::BuyTime: {
      buyTimeState(dt);

      break;
    }

    case State::Game: {
      break;
    }

    case State::Error: {
      break;
    }
  }
}

Server::State Server::getState() const {
  return m_state;
}

uint8_t Server::getConnectedPlayers() const {
  uint8_t count = 0;
  for (uint8_t i = 0; i < MAX_PLAYERS; ++i) {
    if (m_players_states[i].connected == true) {
      ++count;
    }
  }

  return count;
}

int8_t Server::getPlayerIDByIpAddress(const std::string& ip_address) {
  for (uint8_t i = 0; i < MAX_PLAYERS; ++i) {
    if (m_players_states[i].ip_address.toString() == ip_address) {
      return i;
    }
  }

  return -1;
}