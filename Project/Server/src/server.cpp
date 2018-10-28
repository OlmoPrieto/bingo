#include "server.h"

#include "utils.h"

Server::Server() {
  m_state = State::Starting;
  //m_state = State::Error;
  m_connected_players[0] = false;
  m_connected_players[1] = false;

  m_sockets[0].bind(14194);
  m_sockets[0].setBlocking(false);
}

Server::~Server() {

}

void Server::update() {
  switch (m_state) {
    case State::Starting: {
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
              }

              memset(buffer, 0, 1024);
              Message msg((uint64_t)Message::MsgType::ConnectionRequest,
                m_players_connection_state[i] + 1, nullptr);
              msg.fillBuffer(buffer, 1024);
              m_sockets[0].send(buffer, (uint64_t)1024,
                ip_address, client_port);
            }
          }
        }
      }
      else {
        m_state = State::BuyTime;
      }
      break;
    }

    case State::BuyTime: {

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