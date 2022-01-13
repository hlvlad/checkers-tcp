//
// Created by vladvance on 26/12/2021.
//

#ifndef CHECKERS_SERVER_TCP_MESSAGE_H
#define CHECKERS_SERVER_TCP_MESSAGE_H

#include <cstdint>
#include <ostream>
#include "QTcpSocket"

#define MAX_MESSAGE_LEN 10

enum MessageType: uint8_t {
    HANDSHAKE,
    LOBBY_CREATED,
    DISCONNECT,
    MOVE,
    RESIGN,
    ERROR,
    GAME_STARTED
};

enum ErrorType: uint8_t {
    LOBBY_NOT_EXISTS,
    OPPONENT_DISCONNECTED,
    SERVER_DISCONNECTED,
    SERVER_ERROR,
	INVALID_MOVE
};

enum GameFlags: uint8_t {
    EMPTY = 0,
    IM_WHITE = 1 << 0,
};


struct MessageStorage {
    enum MessageType message_type;
    uint8_t len;
    unsigned char payload[MAX_MESSAGE_LEN];
	operator QString() const;
};

enum HandshakeType: uint8_t {
    CREATE_SESSION,
    CONNECT_TO_SESSION
};

struct HandshakeMessage {
    enum HandshakeType handshake_type;
};

struct MoveMessage {
    uint32_t from_to;
};


struct HandshakeResult {
  HandshakeType handshake_type = HandshakeType::CREATE_SESSION;
  uint32_t lobby_id = 0;
};

std::ostream& operator<<(std::ostream& os, const MessageStorage& message_storage);


#endif //CHECKERS_SERVER_TCP_MESSAGE_H
