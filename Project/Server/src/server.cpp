#include "server.h"

#include <thread>

enum class MsgType {
  ConnectionRequest = 0,
};

Server::Server() {
  m_state = State::Starting;
  m_connected_players[0] = false;
  m_connected_players[1] = false;
}

Server::~Server() {

}

void Server::update() {
  m_time1 = m_frame_clock.now();

  switch (m_state) {
    case State::Starting: {
      uint64_t bytes_received = 0;
      uint16_t port = 14194;
      uint8_t buffer[1024];
      memset(buffer, 0, 1024);
      if (m_sockets[0].receive(buffer, (uint64_t)1024, bytes_received,
        sf::IpAddress("127.0.0.1"), port) == sf::Socket::Status::Done) {

        // Check if the packet received contains a connecting request
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