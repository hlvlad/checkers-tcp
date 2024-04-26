
#include "game_session.h"

#include "message_handler.h"

#include <spdlog/spdlog.h>

#include <atomic>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <poll.h>


int check_error_recv(size_t nbytes) {
    if (nbytes <= 0) {
        if (nbytes == 0) {
            // Connection closed
            return -1;
        } else {
            perror("recv");
            return -1;
        }
    }
    return 0;
}


int receive_message(int socket_fd, struct MessageStorage* message) {
    size_t nbytes;
    // Read message type
    nbytes = recv(socket_fd, &(message->message_type), sizeof message->message_type, 0);
    if(check_error_recv(nbytes) < 0) return -1;

    // Read message length
    nbytes = recv(socket_fd, &(message->len), sizeof message->len, 0);
    if(check_error_recv(nbytes) < 0) return -1;

    if(message->len == 0) return 0;

    // Read all payload
    size_t total = 0, bytesleft = message->len;
    while(total < message->len) {
        nbytes = recv(socket_fd, message->payload+total, bytesleft, 0);
        if (check_error_recv(nbytes) < 0) return -1;
        total += nbytes;
        bytesleft -= nbytes;
    }
    return 0;
}

void cleanup_session(SessionData& session_data) {
  session_data.player_sockets[0].close();
  session_data.player_sockets[1].close();
}

void game_session_routine(Socket player1_socket, Socket player2_socket, std::atomic<bool>& is_exit, uint32_t lobby_id) {
  spdlog::info("Started game session thread for lobby {}.", lobby_id);
  SessionData session_data(player1_socket, player2_socket);
  const nfds_t fd_count = 2;
  send_game_started(player1_socket, GameFlags::IM_WHITE);
  send_game_started(player2_socket, GameFlags::NONE);

  struct MessageStorage incoming_message{};

  while (!is_exit) {
	spdlog::info("Waiting for new messages in game session...");
	const int poll_count = poll(session_data.pfds, fd_count, -1);

	if (poll_count == -1) {
	  spdlog::error("Error occurred when polling data.");
	  cleanup_session(session_data);
	  is_exit = true;
	  return;
	}

	for(int socket_number = 0; socket_number < fd_count; ++socket_number) {
	  const auto& pfd = session_data.pfds[socket_number];
	  if(pfd.revents & POLLHUP) {
		spdlog::error("Client closed connection.");
		if(socket_number == PLAYER1_SOCKET) {
		  send_error(player2_socket, OPPONENT_DISCONNECTED);
		} else {
		  send_error(player1_socket, OPPONENT_DISCONNECTED);
		}
		is_exit = true;
	  } else if(pfd.revents & POLLIN) {
		spdlog::info("Reading new message...");
		if(receive_message(pfd.fd, &incoming_message) == -1) {
		  //handle error here
		  spdlog::error("Error occurred when trying to receive message.");
		  if(socket_number == PLAYER1_SOCKET) {
			  send_error(player2_socket, OPPONENT_DISCONNECTED);
		  } else {
			  send_error(player1_socket, OPPONENT_DISCONNECTED);
		  }
		  is_exit = true;
		} else {
		  spdlog::info("Received new session message: {}", message_to_string(incoming_message));
		  session_data.handle_message(SocketNumber(socket_number), incoming_message, is_exit);
		}
	  } else if(pfd.revents) {
		spdlog::error("Unknown error occurred.");
		send_error(player1_socket, SERVER_DISCONNECTED);
		send_error(player2_socket, SERVER_DISCONNECTED);
		is_exit = true;
	  }
	}
  }
  cleanup_session(session_data);
}

SessionData::SessionData(Socket player1_socket, Socket player2_socket) {
  player_sockets[0] = player1_socket;
  player_sockets[1] = player2_socket;
  pfds[0].fd = player1_socket.getSocketFd();
  pfds[1].fd = player2_socket.getSocketFd();
  pfds[0].events = POLLIN;
  pfds[1].events = POLLIN;
  engine.reset();
}

void SessionData::handle_message(SocketNumber socket_number, const struct MessageStorage& message, std::atomic<bool>& is_exit) {
	switch(message.message_type) {
	  case MOVE: {
		Move move;
		move.from = SpotIndex(message.payload[0]);
		move.to = SpotIndex(message.payload[1]);
		move.type = MoveType(message.payload[2]);
		if(engine.is_valid(move)) {
		  engine.make_move(move);
		  send_message(player_sockets[!socket_number], message);
		} else {
		  send_error(player_sockets[0], ErrorType::INVALID_MOVE);
		  send_error(player_sockets[1], ErrorType::INVALID_MOVE);
		  is_exit = true;
		}
		break;
	  }
	  case RESIGN: {
		send_message(player_sockets[!socket_number], message);
		is_exit = true;
	  }
//	  case LOBBY_CREATED:break;
//	  case HANDSHAKE:break;
//	  case DISCONNECT:break;
	  default:break;
	}
}
