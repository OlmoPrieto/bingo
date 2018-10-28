#include "client.h"

#include <thread>

#include "utils.h"

Client::Client() {
  m_state = State::Starting;
  m_socket.bind(14195);
  m_socket.setBlocking(false);
}

Client::~Client() {

}

void Client::update() {
  m_time1 = m_frame_clock.now();

  switch (m_state) {
    case State::Starting: {
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

      break;
    }

    case State::BuyTime: {
      break;
    }

    case State::Game: {
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

float Client::lastFrameTime() const {
  return m_last_frame_time;
}

Client::State Client::getState() const {
  return m_state;
}