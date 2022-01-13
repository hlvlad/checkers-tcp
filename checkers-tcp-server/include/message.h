//
// Created by vladvance on 26/12/2021.
//

#ifndef CHECKERS_SERVER_TCP_MESSAGE_H
#define CHECKERS_SERVER_TCP_MESSAGE_H

#include <cstdint>
#include <mutex>
#include <poll.h>
#include "socket.h"
#include "game_session.h"

#define MAX_MESSAGE_LEN 10

enum MessageType: uint8_t {
    HANDSHAKE, LOBBY_CREATED, DISCONNECT, MOVE, RESIGN, ERROR, GAME_STARTED
};

enum ErrorType: uint8_t {
	LOBBY_NOT_EXISTS,
	OPPONENT_DISCONNECTED,
	SERVER_DISCONNECTED,
	SERVER_ERROR,
	INVALID_MOVE
};

struct MessageStorage {
    enum MessageType message_type;
    uint8_t len;
    unsigned char payload[MAX_MESSAGE_LEN];
};

enum HandshakeType: uint8_t {
    CREATE_SESSION,
    CONNECT_TO_SESSION
};

enum GameFlags: uint8_t {
  NONE = 0,
  IM_WHITE = 1 << 0,
};

struct HandshakeResult {
  HandshakeType handshake_type = HandshakeType::CREATE_SESSION;
  uint32_t lobby_id = 0;
};

void receive_message(const Socket& socket, MessageStorage& message_storage);

void send_message(Socket &socket, const MessageStorage &message_storage);

void send_lobby_created(Socket &socket, uint32_t lobby_id);

void send_game_started(Socket &socket, GameFlags game_flags);

void send_error(Socket &socket, ErrorType error_type);

struct HandshakeResult receive_handshake(const Socket &socket);

std::ostream& operator<<(std::ostream& os, const MessageStorage& message_storage);

std::string message_to_string(const MessageStorage& message);


#endif //CHECKERS_SERVER_TCP_MESSAGE_H
