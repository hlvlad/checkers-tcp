//
// Created by vladvance on 31/12/2021.
//

#include "message.h"
#include "pack.h"
#include "new_misc.h"

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
	default:
	  // should never reach here
	  break;
  }
  return result + "]";
}

std::ostream& operator<<(std::ostream& os, const MessageStorage& message_storage) {
  os << message_to_string(message_storage);
  return os;
}

MessageStorage::operator QString() const {
  return QString::fromStdString(message_to_string(*this));
}
