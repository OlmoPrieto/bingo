#include "server.h"

#include <cmath>

Server::Server() {
  m_state = State::Starting;

  m_sockets[0].bind(14194);
  m_sockets[0].setBlocking(false);

  printf("Server starting\n");

  m_sockets[1].bind(14196);
  m_sockets[1].setBlocking(false);

  // This below causes more problems that it solves.
  // The thing is that reserve causes a memory problem sometimes
  // when trying to std::allocate_and_copy
  //
  // m_cards_numbers[0].reserve(4);  // reserve space for at least 4 cards
  // for (uint8_t i = 0; i < 4; ++i) {
  //   m_cards_numbers[0].operator[](i).reserve(15);  // reserve space for 15 numbers
  // }
  // m_cards_numbers[1].reserve(4);
  // for (uint8_t i = 0; i < 4; ++i) {
  //   m_cards_numbers[1].operator[](i).reserve(15);
  // }
}

Server::~Server() {

}

uint8_t Server::connectionHandshake(uint8_t socket_index, Message::MsgType msg_type) {
  uint64_t bytes_received = 0;
  uint16_t server_port = 14194;
  uint16_t client_port = 14195;
  uint8_t buffer[1024]; // 80 bytes should do, though
  sf::IpAddress ip_address = sf::IpAddress::LocalHost;
  memset(buffer, 0, 1024);
  if (m_sockets[socket_index].receive(buffer, (uint64_t)1024, bytes_received,
    ip_address, client_port) == sf::Socket::Status::Done) {
  
    printf("Got address: %s\n", ip_address.toString().c_str());

    // Check if the received packet contains a connecting request
    uint64_t header = (uint64_t)buffer[0];
    if ((Message::MsgType)header == msg_type) {
      uint64_t data_received = (uint64_t)buffer[sizeof(uint64_t)];
      if (data_received > m_players_connection_state[socket_index]) {
        m_players_connection_state[socket_index] = data_received;
      }

      // if (m_players_connection_state[i] >= 3) {
      //   m_connected_players[i] = true;
      //   //m_players_connection_state[i] = 0;  // to reuse it later
      // }

      memset(buffer, 0, 1024);
      Message msg((uint64_t)msg_type,
        m_players_connection_state[socket_index] + 1, nullptr);
      msg.fillBuffer(buffer, 1024);
      m_sockets[socket_index].send(buffer, (uint64_t)1024,
        ip_address, client_port);

      // if (m_players_connection_state[socket_index] >= 3) {
      //   m_connected_players[socket_index] = true;
      //   m_players_connection_state[socket_index] = 0;  // to reuse it later
      //   // if this instruction is not here in the end, it wouldn't work
      // }
    }
  }
}

void Server::startingState() {
  if (getConnectedPlayers() < 1) {
    for (uint8_t i = 0; i < MAX_PLAYERS; ++i) {
      // uint64_t bytes_received = 0;
      // uint16_t server_port = 14194;
      // uint16_t client_port = 14195;
      // uint8_t buffer[1024]; // 80 bytes should do, though
      // sf::IpAddress ip_address = sf::IpAddress::LocalHost;
      // memset(buffer, 0, 1024);
      // if (m_sockets[0].receive(buffer, (uint64_t)1024, bytes_received,
      //   ip_address, client_port) == sf::Socket::Status::Done) {
      
      //   // Check if the received packet contains a connecting request
      //   uint64_t header = (uint64_t)buffer[0];
      //   if ((Message::MsgType)header == Message::MsgType::ConnectionRequest) {
      //     uint64_t data_received = (uint64_t)buffer[sizeof(uint64_t)];
      //     if (data_received > m_players_connection_state[i]) {
      //       m_players_connection_state[i] = data_received;
      //     }

      //     // if (m_players_connection_state[i] >= 3) {
      //     //   m_connected_players[i] = true;
      //     //   //m_players_connection_state[i] = 0;  // to reuse it later
      //     // }

      //     memset(buffer, 0, 1024);
      //     Message msg((uint64_t)Message::MsgType::ConnectionRequest,
      //       m_players_connection_state[i] + 1, nullptr);
      //     msg.fillBuffer(buffer, 1024);
      //     m_sockets[0].send(buffer, (uint64_t)1024,
      //       ip_address, client_port);

      //     if (m_players_connection_state[i] >= 3) {
      //       m_connected_players[i] = true;
      //       m_players_connection_state[i] = 0;  // to reuse it later
      //       // if this instruction is not here in the end, it wouldn't work
      //     }
      //   }
      // }
      // m_players_connection_state[i] = connectionHandshake(i, 
      //   Message::MsgType::ConnectionRequest);
      connectionHandshake(i, 
        Message::MsgType::ConnectionRequest);
      if (m_players_connection_state[i] >= 3) {
        m_connected_players[i] = true;
        m_players_connection_state[i] = 0;  // to reuse it later
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
    uint16_t server_port = 14194;
    uint16_t client_port = 14195;
    uint8_t buffer[1024]; // 80 bytes should do, though
    sf::IpAddress ip_address = sf::IpAddress::LocalHost;
    memset(buffer, 0, 1024);
    Message msg((uint64_t)Message::MsgType::CurrentBuyingTime,
      (uint64_t)(std::round(m_remaining_buy_time * 0.001f)), nullptr);
    msg.fillBuffer(buffer, 1024);
    m_sockets[i].send(buffer, (uint64_t)1024,
      ip_address, client_port);
  
    // Check for bought cards messages
    bytes_received = 0;
    memset(buffer, 0, 1024);
    if (m_sockets[i].receive(buffer, (uint64_t)1024, bytes_received,
        ip_address, client_port) == sf::Socket::Status::Done) {

      // Check if the received packet contains a card bought message
      uint64_t header = (uint64_t)buffer[0];
      if ((Message::MsgType)header == Message::MsgType::BoughtCards) {
        //uint64_t data_received = (uint64_t)buffer[sizeof(uint64_t)];
        m_cards_bought[i] = (uint64_t)buffer[sizeof(uint64_t)];
      }
    }

    // Check for card numbers
    if (m_cards_bought[i] > 0) {
      bytes_received = 0;
      memset(buffer, 0, 1024);
      if (m_sockets[i].receive(buffer, (uint64_t)1024, bytes_received,
        ip_address, client_port) == sf::Socket::Status::Done) {

        // Check if the received packet contains a card numbers message
        uint64_t header = (uint64_t)buffer[0];
        if ((Message::MsgType)header == Message::MsgType::CardNumbers) {
          uint64_t card_index = (uint64_t)buffer[sizeof(uint64_t)];
          Message msg((uint64_t)Message::MsgType::CardNumbers,
            card_index, buffer + sizeof(uint64_t) * 2);
          msg.getCardNumbers(&m_cards_numbers[i].operator[](card_index));
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
    if (m_connected_players[i] == true) {
      ++count;
    }
  }

  return count;
}