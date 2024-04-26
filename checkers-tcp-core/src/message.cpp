#include "pack.h"

#include "message.h"
#include "board.h"

/**
 * @brief Converts a MessageStorage object to a string representation.
 * 
 * @param message The MessageStorage object to convert.
 * @return The string representation of the MessageStorage object.
 */
std::string message_to_string(const MessageStorage& message) {
  std::string result;
  switch(message.message_type) {
    case MessageType::HANDSHAKE: {
      result += "HANDSHAKE (" + std::to_string(message.len) + " bytes) [";
      switch(HandshakeType(message.payload[0])) {
        case HandshakeType::CREATE_SESSION:
          result += "CREATE_SESSION";
          break;
        case HandshakeType::CONNECT_TO_SESSION:
          result += "CONNECT_TO_SESSION, ";
          result += "lobby_id: " + std::to_string(unpacku32(&message.payload[1]));
          break;
        default:
          break;
      }
      break;
    }
    case MessageType::LOBBY_CREATED: {
        result += "LOBBY_CREATED (" + std::to_string(message.len) + " bytes) [";
        uint32_t lobby_id = unpacku32(message.payload);
        result += "lobby_id: " + std::to_string(lobby_id);
        break;
    }
    case MessageType::GAME_STARTED: {
        result += "GAME_STARTED (" + std::to_string(message.len) + " bytes) [";
        if(GameFlags::IM_WHITE & GameFlags(message.payload[0])) {
            result += "IM_WHITE";
        }
        break;
    }
    case MessageType::MOVE: {
        result += "MOVE (" + std::to_string(message.len) + " bytes) [";
        SpotIndex from = SpotIndex(message.payload[0]);
        SpotIndex to = SpotIndex(message.payload[1]);
        MoveType move_type = MoveType(message.payload[2]);
        result += "from: " + std::to_string(from) + ", to: " + std::to_string(to) + ", move_type: ";
        switch(move_type) {
        case NORMAL:result += "NORMAL"; break;
        case CAPTURE:result += "CAPTURE"; break;
        case PROMOTION:result += "PROMOTION"; break;
        case CAPTURE_PROMOTION:result += "CAPTURE_PROMOTION"; break;
        }
        break;
    }
    case MessageType::DISCONNECT:
      result += "DISCONNECT (" + std::to_string(message.len) + " bytes) [";
      break;
    case MessageType::RESIGN:
      result += "RESIGN (" + std::to_string(message.len) + " bytes) [";
      break;
    case MessageType::ERROR:
      result += "ERROR (" + std::to_string(message.len) + " bytes) [";
      switch(ErrorType(message.payload[0])) {
        case ErrorType::LOBBY_NOT_EXISTS: result += "LOBBY_NOT_EXISTS"; break;
        case ErrorType::SERVER_ERROR: result += "SERVER_ERROR"; break;
        case ErrorType::SERVER_DISCONNECTED: result += "SERVER_DISCONNECTED"; break;
        case ErrorType::OPPONENT_DISCONNECTED: result += "OPPONENT_DISCONNECTED"; break;
        case ErrorType::INVALID_MOVE: result += "INVALID_MOVE"; break;
      }
      break;
    default:
      // should never reach here
      break;
  }
  return result + "]";
}

/**
 * @brief Overloaded stream insertion operator for MessageStorage objects.
 * 
 * @param os The output stream.
 * @param message_storage The MessageStorage object to insert into the stream.
 * @return The modified output stream.
 */
std::ostream& operator<<(std::ostream& os, const MessageStorage& message_storage) {
  os << message_to_string(message_storage);
  return os;
}
