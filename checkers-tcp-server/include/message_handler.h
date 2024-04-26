/**
 * @file message_handler.h
 * @brief This file contains the declarations of functions and structures related to message handling.
 */
#pragma once

#include "socket.h"
#include "message.h"

/**
 * @brief Receives a message from the socket and stores it in the message storage.
 * 
 * @param socket The socket to receive the message from.
 * @param message_storage The storage to store the received message.
 */
void receive_message(const Socket& socket, MessageStorage& message_storage);

/**
 * @brief Sends a message through the socket.
 * 
 * @param socket The socket to send the message through.
 * @param message_storage The message to send.
 */
void send_message(Socket &socket, const MessageStorage &message_storage);

/**
 * @brief Sends a lobby created message through the socket.
 * 
 * @param socket The socket to send the message through.
 * @param lobby_id The ID of the created lobby.
 */
void send_lobby_created(Socket &socket, uint32_t lobby_id);

/**
 * @brief Sends a game started message through the socket.
 * 
 * @param socket The socket to send the message through.
 * @param game_flags The flags indicating the game settings.
 */
void send_game_started(Socket &socket, GameFlags game_flags);

/**
 * @brief Sends an error message through the socket.
 * 
 * @param socket The socket to send the message through.
 * @param error_type The type of the error.
 */
void send_error(Socket &socket, ErrorType error_type);

/**
 * @brief Receives a handshake result from the socket.
 * 
 * @param socket The socket to receive the handshake result from.
 * @return The received handshake result.
 */
struct HandshakeResult receive_handshake(const Socket &socket);

/**
 * @brief Overloads the << operator to output the contents of the message storage to the output stream.
 * 
 * @param os The output stream.
 * @param message_storage The message storage to output.
 * @return The modified output stream.
 */
std::ostream& operator<<(std::ostream& os, const MessageStorage& message_storage);

/**
 * @brief Converts the message storage to a string representation.
 * 
 * @param message The message storage to convert.
 * @return The string representation of the message storage.
 */
std::string message_to_string(const MessageStorage& message);
