#pragma once

#include "checkers_engine.h"
#include "socket.h"
#include "message.h"

#include <mutex>
#include <poll.h>
#include <atomic>


enum SocketNumber {
  PLAYER1_SOCKET,
  PLAYER2_SOCKET
};

struct SessionData {
  checkers_engine engine;
  Socket player_sockets[2];
  struct pollfd pfds[2]{};
  std::mutex session_mutex;

  SessionData(Socket player1_socket, Socket player2_socket);
  void handle_message(SocketNumber socket_number, const struct MessageStorage &message, std::atomic<bool>& is_exit);
};

void game_session_routine(Socket player1_socket, Socket player2_socket, std::atomic<bool> &is_exit, uint32_t lobby_id);
