#ifndef __SERVER_H__
#define __SERVER_H__

#include <SFML/Network/UdpSocket.hpp>

constexpr uint8_t MAX_PLAYERS = 2;

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

  void update(float dt);
  float lastFrameTime() const;
  State getState() const;
  uint8_t getConnectedPlayers() const;

private:
  void startingState();
  void buyTimeState(float dt);
  void gameState();
  void errorState();

  sf::UdpSocket m_sockets[2];
  State m_state;
  float m_remaining_buy_time = 30.0f * 1000.0f; // 30 seconds * milliseconds/seconds
  uint8_t m_players_connection_state[MAX_PLAYERS] = { 0 };
  bool m_connected_players[MAX_PLAYERS] = { false };
};

#endif // __SERVER_H__