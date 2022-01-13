//
// Created by vladvance on 25/12/2021.
//

#ifndef CHECKERS_SERVER_TCP_GAME_SESSION_H
#define CHECKERS_SERVER_TCP_GAME_SESSION_H

#include <mutex>
#include <poll.h>
#include <atomic>

//#include "engine.h"
#include "checkers_engine.h"
#include "socket.h"
#include "message.h"

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
  void handle_message(SocketNumber socket_number, const struct MessageStorage &message);
};

void game_session_routine(Socket player1_socket, Socket player2_socket, std::atomic<bool> &is_exit, uint32_t lobby_id);

#endif //CHECKERS_SERVER_TCP_GAME_SESSION_H
