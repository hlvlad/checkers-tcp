#pragma once

#include <cstdint>
#include <ostream>

/**
 * @brief The maximum length of a message.
 */
constexpr size_t MAX_MESSAGE_LEN = 10;

/**
 * @brief Enumerates the types of messages.
 */
enum MessageType: uint8_t {
    HANDSHAKE,          /**< Handshake message type. */
    LOBBY_CREATED,      /**< Lobby created message type. */
    DISCONNECT,         /**< Disconnect message type. */
    MOVE,               /**< Move message type. */
    RESIGN,             /**< Resign message type. */
    ERROR,              /**< Error message type. */
    GAME_STARTED        /**< Game started message type. */
};

/**
 * @brief Enumerates the types of errors.
 */
enum ErrorType: uint8_t {
    LOBBY_NOT_EXISTS,       /**< Lobby does not exist error type. */
    OPPONENT_DISCONNECTED,  /**< Opponent disconnected error type. */
    SERVER_DISCONNECTED,    /**< Server disconnected error type. */
    SERVER_ERROR,           /**< Server error type. */
    INVALID_MOVE            /**< Invalid move error type. */
};

/**
 * @brief Enumerates the game flags.
 */
enum GameFlags: uint8_t {
    NONE = 0,          /**< None game flag. */
    IM_WHITE = 1 << 0   /**< I am white game flag. */
};

/**
 * @brief Structure for storing a message.
 */
struct MessageStorage {
    enum MessageType message_type;   /**< The type of the message. */
    uint8_t len;                     /**< The length of the message. */
    unsigned char payload[MAX_MESSAGE_LEN];   /**< The payload of the message. */
};

/**
 * @brief Enumerates the types of handshake messages.
 */
enum HandshakeType: uint8_t {
    CREATE_SESSION,     /**< Create session handshake type. */
    CONNECT_TO_SESSION  /**< Connect to session handshake type. */
};

/**
 * @brief Structure for a handshake message.
 */
struct HandshakeMessage {
    enum HandshakeType handshake_type;   /**< The type of the handshake message. */
};

/**
 * @brief Structure for a move message.
 */
struct MoveMessage {
    uint32_t from_to;   /**< The move from-to value. */
};

/**
 * @brief Structure for the result of a handshake.
 */
struct HandshakeResult {
    HandshakeType handshake_type = HandshakeType::CREATE_SESSION;   /**< The type of the handshake. */
    uint32_t lobby_id = 0;   /**< The ID of the lobby. */
};

/**
 * @brief Overload of the output stream operator for MessageStorage.
 * @param os The output stream.
 * @param message_storage The message storage object.
 * @return The output stream.
 */
std::ostream& operator<<(std::ostream& os, const MessageStorage& message_storage);

/**
 * @brief Converts a MessageStorage object to a string representation.
 * 
 * @param message The MessageStorage object to convert.
 * @return The string representation of the MessageStorage object.
 */
std::string message_to_string(const MessageStorage& message);
