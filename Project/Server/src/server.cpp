#include "server.h"

#include <cmath>

#include "utils.h"

Server::Server() {
  m_state = State::Starting;
  m_connected_players[0] = false;
  m_connected_players[1] = false;

  m_sockets[0].bind(14194);
  m_sockets[0].setBlocking(false);
}

Server::~Server() {

}

void Server::startingState() {
  if (getConnectedPlayers() < 1) {
    for (uint8_t i = 0; i < MAX_PLAYERS; ++i) {
      uint64_t bytes_received = 0;
      uint16_t server_port = 14194;
      uint16_t client_port = 14195;
      uint8_t buffer[1024]; // 80 bytes should do, though
      sf::IpAddress ip_address = sf::IpAddress::LocalHost;
      memset(buffer, 0, 1024);
      if (m_sockets[0].receive(buffer, (uint64_t)1024, bytes_received,
        ip_address, client_port) == sf::Socket::Status::Done) {
      
        // Check if the received packet contains a connecting request
        uint64_t header = (uint64_t)buffer[0];
        if ((Message::MsgType)header == Message::MsgType::ConnectionRequest) {
          uint64_t data_received = (uint64_t)buffer[sizeof(uint64_t)];
          if (data_received > m_players_connection_state[i]) {
            m_players_connection_state[i] = data_received;
          }

          if (m_players_connection_state[i] >= 3) {
            m_connected_players[i] = true;
            //m_players_connection_state[i] = 0;  // to reuse it later
          }

          memset(buffer, 0, 1024);
          Message msg((uint64_t)Message::MsgType::ConnectionRequest,
            m_players_connection_state[i] + 1, nullptr);
          msg.fillBuffer(buffer, 1024);
          m_sockets[0].send(buffer, (uint64_t)1024,
            ip_address, client_port);

          // if (m_players_connection_state[i] >= 3) {
          //   m_connected_players[i] = true;
          //   m_players_connection_state[i] = 0;  // to reuse it later
          //   // if this instruction is not here in the end, it wouldn't work
          // }
        }
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