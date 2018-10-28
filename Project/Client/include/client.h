#ifndef __CLIENT_H__
#define __CLIENT_H__

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
  State getState() const;

private:
  sf::UdpSocket m_socket;
  State m_state;
  uint8_t m_connection_state = 0;
  bool m_connected = false;
};

#endif  // __CLIENT_H__