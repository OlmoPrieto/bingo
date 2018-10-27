#include "server.h"

#include <thread>

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
  m_time1 = m_frame_clock.now();

  switch (m_state) {
    case State::Starting: {
      uint8_t counter = 0;
      while (counter < 3) { // Go throught the 3 states of the connection (simple handshake)
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
            if (data_received > counter) {
              counter = data_received;
            }
            printf("Received state: %lu\n", counter);
            // when counter reaches 3 (the end of the ACK),
            // the server will send a last invalid packet 
            // and will exit the while() 

            memset(buffer, 0, 1024);
            Message msg((uint64_t)Message::MsgType::ConnectionRequest,
              counter + 1, nullptr);
            msg.fillBuffer(buffer, 1024);
            m_sockets[0].send(buffer, (uint64_t)1024,
              ip_address, client_port);
          
          }
        }
      }

      m_connected_players[0] = true;
      m_state = State::BuyTime;

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


  // Lock tickrate to 60fps
  m_time2 = m_frame_clock.now();
  m_last_frame_time = std::chrono::duration_cast<std::chrono::duration<float>>(m_time2 - m_time1).count();
  m_time1 = m_frame_clock.now();

  // Sleep only for half of the remaining time to the target,
  // because the SO may leave the thread to sleep for more
  // than you specified.
  if (m_last_frame_time < m_target_frame_time) {
    std::this_thread::sleep_for(std::chrono::duration<float, std::milli>((m_target_frame_time - m_last_frame_time) * 0.5f));
  }
  // Spinlock the rest of the frame time to get the precise target
  while (m_last_frame_time <= m_target_frame_time) {
    m_time2 = m_frame_clock.now();
    m_last_frame_time += std::chrono::duration_cast<std::chrono::duration<float>>(m_time2 - m_time1).count();
  }
}

float Server::lastFrameTime() const {
  return m_last_frame_time;
}

Server::State Server::getState() const {
  return m_state;
}