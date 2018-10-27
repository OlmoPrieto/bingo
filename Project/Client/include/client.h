#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <chrono>

#include <SFML/Network/UdpSocket.hpp>

class Client {
public:

  enum class State {
    Starting = 0,
    BuyTime,
    Game,
  };

  Client();
  ~Client();

  void update();
  float lastFrameTime() const;
  State getState() const;

private:
  sf::UdpSocket m_socket;
  std::chrono::high_resolution_clock m_frame_clock;
  std::chrono::high_resolution_clock::time_point m_time1;
  std::chrono::high_resolution_clock::time_point m_time2;
  State m_state;
  float m_target_frame_time = 16.6667f; // Tickrate of 60 fps
  float m_last_frame_time = 0.0f;
};

#endif  // __CLIENT_H__