#include "socket.h"
#include "message.h"

void receive_message(const Socket& socket, MessageStorage& message_storage);

void send_message(Socket &socket, const MessageStorage &message_storage);

void send_lobby_created(Socket &socket, uint32_t lobby_id);

void send_game_started(Socket &socket, GameFlags game_flags);

void send_error(Socket &socket, ErrorType error_type);

struct HandshakeResult receive_handshake(const Socket &socket);

std::ostream& operator<<(std::ostream& os, const MessageStorage& message_storage);

std::string message_to_string(const MessageStorage& message);
