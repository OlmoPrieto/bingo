#include "server.h"

#include <cmath>

Server::Server() {
  m_state = State::Starting;

  m_socket.bind(14194);
  m_socket.setBlocking(false);
}

Server::~Server() {

}

void Server::connectionHandshake(Message::MsgType msg_type, uint8_t* other_buffer,
  sf::IpAddress* remote_address) {
  
  uint64_t bytes_received = 0;
  uint8_t* buffer = (uint8_t*)malloc(1024); // 80 bytes should do, though
  memset(buffer, 0, 1024);
  
  sf::IpAddress ip_address;
  if (remote_address != nullptr) {
    ip_address = *remote_address;
  }

  bool has_package = false;
  if (other_buffer != nullptr) {
    has_package = true;
    free(buffer);
    buffer = other_buffer;
  }
  else {
    has_package = m_socket.receive(buffer, (uint64_t)1024, bytes_received,
      ip_address, m_client_port) == sf::Socket::Status::Done;
  }

  if (has_package == true) {
    int8_t player_id = getPlayerIDByIpAddress(ip_address.toString());
    if (player_id == -1) {
      player_id = 0;
      for (uint8_t i = 0; i < MAX_PLAYERS; ++i) {
        if (m_players_states[i].player_id != -1) {
          ++player_id;
        }
      }
      m_players_states[player_id].ip_address = ip_address;
      m_players_states[player_id].player_id = player_id;
      player_id = m_players_states[player_id].player_id;
    }

    // Check if the received packet contains the requested message
    uint64_t header = (uint64_t)buffer[0];
    if ((Message::MsgType)header == msg_type) {
      uint64_t data_received = (uint64_t)buffer[sizeof(uint64_t)];
      if (data_received > m_players_states[player_id].connection_state) {
        m_players_states[player_id].connection_state = data_received;
      }

      memset(buffer, 0, 1024);
      Message msg((uint64_t)msg_type,
        m_players_states[player_id].connection_state + 1, nullptr);
      msg.fillBuffer(buffer, 1024);
      m_socket.send(buffer, (uint64_t)1024,
        m_players_states[player_id].ip_address, m_client_port);
    }
  }

  // If the passed buffer is null, then memory was reserved for buffer
  if (other_buffer == nullptr) {
    free(buffer);
  }
}

void Server::startingState() {
  if (getConnectedPlayers() < 1) {
    for (uint8_t i = 0; i < MAX_PLAYERS; ++i) {
      connectionHandshake(Message::MsgType::ConnectionRequest);
      if (m_players_states[i].connection_state >= 3) {
        m_players_states[i].connected = true;
        m_players_states[i].connection_state = 0; // to reuse it later
      }
    }
  }
  else {
    m_state = State::BuyTime;
  }
}

bool Server::checkCard(uint8_t player_index, uint8_t card_index) {
  uint8_t numbers[15] = { 0 };
  for (uint8_t i = 0; i < 30; i += 2) {
    uint8_t index = (uint8_t)(i * 0.5f);
    numbers[index] = m_players_states[player_index].cards_numbers[card_index][i];
    printf("%u ", numbers[index]);
    if (index % 5 + 1 == 0) {
      printf("\n");
    }
    for (uint8_t j = 0; j < index; ++j) {
      if (index != j) {
        if (numbers[index] == numbers[j] || (numbers[index] < 1 || numbers[index] > 90)) {
          printf("ERRRRRRRRRORRRRRRRR, number repeated or not between 1 and 90\n");
          return false;
        }
      }
    }
  }

  return true;
}

void Server::buyTimeState(float dt) {
  if (m_remaining_buy_time - dt >= 0.0f) {
    m_remaining_buy_time -= dt * 0.5f;
  }
  // TODO: else {} start over 30 seconds again if none of the players bought a card

  // Run the loop up to connected players' times so every player 
  // has a chance to check its packets.
  for (uint8_t i = 0; i < getConnectedPlayers(); ++i) {
    // Send remaining time information
    uint64_t bytes_received = 0;
    uint8_t buffer[1024]; // 80 bytes should do, though
    sf::IpAddress ip_address;
    memset(buffer, 0, 1024);
    Message msg((uint64_t)Message::MsgType::CurrentBuyingTime,
      (uint64_t)(std::round(m_remaining_buy_time * 0.001f)), nullptr);
    msg.fillBuffer(buffer, 1024);
    m_socket.send(buffer, (uint64_t)1024,
      m_players_states[i].ip_address, m_client_port);
  
    // Check for bought cards messages && its handshake
    bytes_received = 0;
    memset(buffer, 0, 1024);
    if (m_socket.receive(buffer, (uint64_t)1024, bytes_received,
      ip_address, m_client_port) == sf::Socket::Status::Done) {

      int8_t player_id = getPlayerIDByIpAddress(ip_address.toString());

      if (m_players_states[player_id].cards_bought == 0) {
        // However, a player can "buy" zero cards and when the time runs out,
        // the game will continue (if the other player bought at least one).
        // So maybe the code will keep entering here for this particular player
        // but when the time runs out the code will continue.
      
        // Check if the received packet contains a card bought message
        uint64_t header = (uint64_t)buffer[0];
        if ((Message::MsgType)header == Message::MsgType::BoughtCards) {
          m_players_states[player_id].cards_bought = 
            (uint64_t)buffer[sizeof(uint64_t)];
          connectionHandshake(Message::MsgType::BoughtCardsConfirmation);
        }
      }
      else {
        if (m_players_states[player_id].connection_state > 0) {
          connectionHandshake(Message::MsgType::BoughtCardsConfirmation, buffer,
            &ip_address);

          if (m_players_states[player_id].connection_state >= 3) {
            // Player X cards bought confirmed
            printf("Player %d bought %u cards\n", 
              m_players_states[player_id].player_id,
              m_players_states[player_id].cards_bought);

            m_players_states[player_id].connection_state = 0;
            m_players_states[player_id].check_cards_numbers = true;
          }
        }
      }
    }

    // Check for card numbers
    if (m_players_states[i].cards_bought > 0 && 
      m_players_states[i].check_cards_numbers == true) {

      bytes_received = 0;
      memset(buffer, 0, 1024);
      if (m_socket.receive(buffer, (uint64_t)1024, bytes_received,
        ip_address, m_client_port) == sf::Socket::Status::Done) {

        int8_t player_id = getPlayerIDByIpAddress(ip_address.toString());

        if (m_players_states[player_id].connection_state == 0 && 
          m_players_states[player_id].has_card_numbers_message == false) {
          
          // Check if the received packet contains a card numbers message
          uint64_t header = (uint64_t)buffer[0];
          printf("MsgType: %d\n", header);
          if ((Message::MsgType)header == Message::MsgType::CardNumbers) {
            uint64_t cards_amount = (uint64_t)buffer[sizeof(uint64_t)];
            
            Message msg((uint64_t)Message::MsgType::CardNumbers,
              cards_amount, buffer + sizeof(uint64_t) * 2);
            msg.getCardNumbers(&m_players_states[player_id].cards_numbers, 
              cards_amount);

            m_players_states[player_id].has_card_numbers_message = true;

            for (uint8_t j = 0; j < cards_amount; ++j) {
              checkCard(player_id, j);
            }

            connectionHandshake(Message::MsgType::CardNumbersConfirmation);
          }
        }
        else {
          printf("Blyat\n");
          connectionHandshake(Message::MsgType::CardNumbersConfirmation, buffer,
            &ip_address);

          if (m_players_states[player_id].connection_state >= 3) {
            m_players_states[player_id].connection_state = 0;
          }
        }
      }
      uint64_t header = (uint64_t)buffer[0];
      printf("MsgType: %d\n", header);
    }
  }
}

void Server::update(float dt) {
  switch (m_state) {
    case State::Starting: {
      startingState();

      break;
    }

    case State::BuyTime: {
      buyTimeState(dt);

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
    if (m_players_states[i].connected == true) {
      ++count;
    }
  }

  return count;
}

int8_t Server::getPlayerIDByIpAddress(const std::string& ip_address) {
  for (uint8_t i = 0; i < MAX_PLAYERS; ++i) {
    if (m_players_states[i].ip_address.toString() == ip_address) {
      return i;
    }
  }

  return -1;
}