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

private:
  struct PlayerState {
    std::vector<std::vector<uint8_t> > cards_numbers;
    sf::IpAddress ip_address;
    uint8_t connection_state = 0;
    uint8_t cards_bought = 0;
    bool connected = false;
  };

  void connectionHandshake(uint8_t socket_index, 
    Message::MsgType msg_type = Message::MsgType::Handshake);
  void startingState();
  void buyTimeState(float dt);
  void gameState();
  void errorState();

  sf::UdpSocket m_sockets[MAX_PLAYERS];
  //std::vector<std::vector<uint8_t> > m_cards_numbers[MAX_PLAYERS];  // one for each player
  PlayerState m_players_states[MAX_PLAYERS];
  State m_state;
  float m_remaining_buy_time = 30.0f * 1000.0f; // 30 seconds * milliseconds/seconds
  //uint8_t m_players_connection_state[MAX_PLAYERS] = { 0 };
  //uint8_t m_cards_bought[MAX_PLAYERS] = { 0 };
  //bool m_connected_players[MAX_PLAYERS] = { false };
};

#endif // __SERVER_H__