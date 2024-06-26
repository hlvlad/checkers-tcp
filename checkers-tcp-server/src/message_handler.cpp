/**
 * @file message_handler.cpp
 * @brief Implementation of message handling functions.
 */

#include "message_handler.h"

#include "message.h"
#include "pack.h"
#include "board.h"

#include <spdlog/spdlog.h>

#include <cstring>

/**
 * @brief Converts a MessageStorage object to a string representation.
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
			if(GameFlags(message.payload[0]) & GameFlags::IM_WHITE) {
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
 * @param os The output stream.
 * @param message_storage The MessageStorage object to insert into the stream.
 * @return The output stream.
 */
std::ostream& operator<<(std::ostream& os, const MessageStorage& message_storage) {
	os << message_to_string(message_storage);
	return os;
}

/**
 * @brief Receives a message from a socket and stores it in a MessageStorage object.
 * @param socket The socket to receive the message from.
 * @param message_storage The MessageStorage object to store the received message.
 */
void receive_message(const Socket &socket, MessageStorage &message_storage) {
	uint8_t buf[2];
	// Receive message type and length
	socket.receiveAll(buf, 2);
	message_storage.message_type = MessageType(buf[0]);
	message_storage.len = buf[1];
	// Receive payload
	socket.receiveAll(message_storage.payload, message_storage.len);
	spdlog::info("Received message: {}", message_to_string(message_storage));
}

/**
 * @brief Sends a message through a socket.
 * @param socket The socket to send the message through.
 * @param message_storage The MessageStorage object containing the message to send.
 */
void send_message(Socket &socket, const MessageStorage &message_storage) {
	unsigned char buf[2 + MAX_MESSAGE_LEN];
	buf[0] = message_storage.message_type;
	buf[1] = message_storage.len;
	std::memcpy(buf+2, message_storage.payload, message_storage.len);
	// Send message type and length
	socket.sendAll((char*)buf, 2+message_storage.len);
	spdlog::info("Sent message: {}", message_to_string(message_storage));
}

/**
 * @brief Receives a handshake message from a socket and returns the handshake result.
 * @param socket The socket to receive the handshake message from.
 * @return The handshake result.
 */
struct HandshakeResult receive_handshake(const Socket &socket) {
	MessageStorage message_storage{};
	receive_message(socket, message_storage);
	auto handshake_type = HandshakeType(message_storage.payload[0]);
	if (handshake_type == HandshakeType::CONNECT_TO_SESSION) {
		uint32_t lobby_id = unpacku32(&message_storage.payload[1]);
		return HandshakeResult{handshake_type, lobby_id};
	}
	return HandshakeResult{handshake_type};
}

/**
 * @brief Sends a lobby created message through a socket.
 * @param socket The socket to send the lobby created message through.
 * @param lobby_id The ID of the created lobby.
 */
void send_lobby_created(Socket &socket, uint32_t lobby_id) {
	MessageStorage message{MessageType::LOBBY_CREATED, 4};
	packi32(message.payload, lobby_id);
	send_message(socket, message);
}

/**
 * @brief Sends a game started message through a socket.
 * @param socket The socket to send the game started message through.
 * @param game_flags The game flags indicating the game state.
 */
void send_game_started(Socket &socket, GameFlags game_flags) {
	MessageStorage message{MessageType::GAME_STARTED, 1};
	message.payload[0] = game_flags;
	send_message(socket, message);
}

/**
 * @brief Sends an error message through a socket.
 * @param socket The socket to send the error message through.
 * @param error The type of error.
 */
void send_error(Socket &socket, ErrorType error) {
	MessageStorage message{MessageType::ERROR, 1};
	message.payload[0] = error;
	send_message(socket, message);
}

void send_no_lobby(Socket socket) {
	// TODO: Implement send_no_lobby function
}

