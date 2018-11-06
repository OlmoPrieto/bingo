#ifndef __SERVER_H__
#define __SERVER_H__

#include <vector>

#include <SFML/Network/UdpSocket.hpp>

#include "utils.h"

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
  State getState() const;
  uint8_t getConnectedPlayers() const;
  int8_t getPlayerIDByIpAddress(const std::string& ip_address);

private:
  struct PlayerState {
    std::vector<std::vector<uint8_t> > cards_numbers;
    sf::IpAddress ip_address = sf::IpAddress::Any;
    int8_t player_id = -1;
    uint8_t connection_state = 0;
    uint8_t cards_bought = 0;
    bool connected = false;
  };

  void connectionHandshake(Message::MsgType msg_type = 
    Message::MsgType::Handshake);
  void startingState();
  void buyTimeState(float dt);
  void gameState();
  void errorState();

  sf::UdpSocket m_socket;
  PlayerState m_players_states[MAX_PLAYERS];
  State m_state;
  float m_remaining_buy_time = 30.0f * 1000.0f; // 30 seconds * milliseconds/seconds
};

#endif // __SERVER_H__