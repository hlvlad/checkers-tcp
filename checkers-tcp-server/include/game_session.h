/**
 * @file game_session.h
 * @brief Contains the declaration of the game session related structures and functions.
 */

#pragma once

#include "checkers_engine.h"
#include "socket.h"
#include "message.h"

#include <mutex>
#include <poll.h>
#include <atomic>

/**
 * @brief Enum representing the socket numbers for player 1 and player 2.
 */
enum SocketNumber {
  PLAYER1_SOCKET, /**< Socket number for player 1. */
  PLAYER2_SOCKET  /**< Socket number for player 2. */
};

/**
 * @brief Structure representing the data for a game session.
 */
struct SessionData {
  checkers_engine engine; /**< The checkers engine for the game session. */
  Socket player_sockets[2]; /**< Array of player sockets. */
  struct pollfd pfds[2]{}; /**< Array of poll file descriptors. */
  std::mutex session_mutex; /**< Mutex for session synchronization. */

  /**
   * @brief Constructor for SessionData.
   * @param player1_socket The socket for player 1.
   * @param player2_socket The socket for player 2.
   */
  SessionData(Socket player1_socket, Socket player2_socket);

  /**
   * @brief Handles the incoming message for the specified socket.
   * @param socket_number The socket number.
   * @param message The message storage.
   * @param is_exit Atomic flag indicating if the session should exit.
   */
  void handle_message(SocketNumber socket_number, const struct MessageStorage &message, std::atomic<bool>& is_exit);
};

/**
 * @brief Function for running the game session routine.
 * @param player1_socket The socket for player 1.
 * @param player2_socket The socket for player 2.
 * @param is_exit Atomic flag indicating if the session should exit.
 * @param lobby_id The ID of the lobby.
 */
void game_session_routine(Socket player1_socket, Socket player2_socket, std::atomic<bool> &is_exit, uint32_t lobby_id);
