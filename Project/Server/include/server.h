#ifndef __SERVER_H__
#define __SERVER_H__

#include <chrono>

#include <SFML/Network/UdpSocket.hpp>

class Server {
public:
  enum class State {
    Starting = 0,
    BuyTime,
    Game,
    Error,  // Any of the players disconnected
  };

  Server();
  ~Server();

  void update();
  float lastFrameTime() const;

private:
  sf::UdpSocket m_sockets[2];
  std::chrono::high_resolution_clock m_frame_clock;
  std::chrono::high_resolution_clock::time_point m_time1;
  std::chrono::high_resolution_clock::time_point m_time2;
  State m_state;
  float m_target_frame_time = 16.6667f; // Tickrate of 60 fps
  float m_last_frame_time = 0.0f;
  bool m_connected_players[2];
};

#endif // __SERVER_H__