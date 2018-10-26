#include <vector>

class Message {
public:
  enum class MsgType {
    ConnectionRequest = 0,  // + Status   -> [1, 3]
    CurrentBuyingTime,      // + Current  -> [0, X]
    BoughtCards,            // + Player   -> [1, 2]   + Amount   -> [0, 4]
    CardNumbers,            // + CardID   -> [0, 3]   + 15 numbers separated by '#'
    NumberWithdrawn,        // + Current  -> [1, 90]
    LineScored,             // + Player   -> [1, 2]
    BingoScored,            // + Player   -> [1, 2]
  };

  Message() {

  }
  Message(uint64_t header, uint64_t data, uint8_t* extra) {
    m_header = header;
    m_data = data;
    if (extra != nullptr) {
      memcpy(m_extra, extra, sizeof(m_extra));
    }
  }
  ~Message() {

  }

  Message::MsgType getMessageType() const {
    return (Message::MsgType)m_header;
  }

  void fillBuffer(uint8_t* buffer, uint64_t buffer_size) {
    uint64_t header_size = sizeof(m_header);
    uint64_t data_size = sizeof(m_data);
    uint64_t extra_size = sizeof(m_extra);
    uint64_t message_size = header_size + data_size + extra_size;
    if (buffer != nullptr && message_size <= buffer_size) {
      memcpy(buffer, &m_header, header_size);
      memcpy(buffer + header_size, &m_data, data_size);
      memcpy(buffer + header_size + data_size, &m_extra, extra_size);
    }
    else {
      memset(buffer, 0, buffer_size);
    }
  }

  void setCardNumbersField(const std::vector<uint8_t>& numbers) {
    if ((MsgType)m_header != MsgType::CardNumbers) {
      printf("ERROR: trying to set extra field in a message that is not of type CardNumbers\n");
    }
    if (numbers.size() > 15) {
      printf("ERROR: more than 15 numbers provided for the card\n");
    }

    for (uint64_t i = 0; i < numbers.size(); i += 2) {
      m_extra[(uint8_t)i] = numbers[i];
      m_extra[(uint8_t)i] = (uint8_t)('#');
    }
  }

  uint64_t m_header = 0;
  uint64_t m_data = 0;
  uint8_t  m_extra[64] = { 0 };
};