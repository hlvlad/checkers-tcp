#define FMT_HEADER_ONLY
#define FMTLOG_HEADER_ONLY

#include "game_session.h"
#include "message_handler.h"

#include "spdlog/spdlog.h"

#include <atomic>
#include <iostream>
#include <random>
#include <thread>
#include <unordered_map>
#include <csignal>


Socket server_socket;

uint32_t random_id() {
  static std::random_device rd;
  static std::mt19937 mt(rd());
  static std::uniform_int_distribution<uint32_t> dist(1, UINT32_MAX);
  return dist(rd);
}

struct LobbyInfo {

  uint32_t lobby_id = 0;
  Socket player1{};
  Socket player2{};
  std::atomic<bool> is_closed = false;

  LobbyInfo(uint32_t lobby_id, Socket _player1) : lobby_id(lobby_id), player1(_player1) {}

  explicit LobbyInfo() = default;
  explicit LobbyInfo(Socket player_socket) : player1(player_socket) {}

  [[nodiscard]] bool is_lobby_full() const {
	return player1.getSocketFd() != -1 && player2.getSocketFd() != -1;
  }

  void add_player2(Socket player_socket) {
	player2 = player_socket;
  }

  void start_game() {
	std::thread session_thread(game_session_routine, player1, player2, std::ref(is_closed), lobby_id);
	session_thread.detach();
  }
};


struct LobbiesList {
  std::unordered_map <uint32_t, LobbyInfo> lobbies;
  std::mutex list_mutex;

  LobbiesList() = default;

  uint32_t add_lobby(Socket player_sock) {
	std::scoped_lock<std::mutex> lock(list_mutex);

	uint32_t lobby_id = random_id();
	while (lobbies.find(lobby_id) != lobbies.end()) {
	  lobby_id = random_id();
	}
	spdlog::info("Adding new lobby with id: {} ({:X})", lobby_id, lobby_id);
	lobbies.emplace(std::piecewise_construct,
					std::forward_as_tuple(lobby_id),
					std::forward_as_tuple(lobby_id, player_sock));
	return lobby_id;
  }

  int add_player_to_lobby(Socket player_socket, uint32_t lobby_id) {
	auto lobby_it = lobbies.find(lobby_id);
	if (lobby_it == lobbies.end()) {
	  return -1;
	}
	spdlog::info("Adding new player {} to lobby with id {}", player_socket.getAddressString(), lobby_id);
	lobby_it->second.add_player2(player_socket);
	lobby_it->second.start_game();
	return 0;
  }

  void remove_lobby(int lobby_id) {
	std::scoped_lock<std::mutex> lock(list_mutex);
	lobbies.at(lobby_id).is_closed = true;
	lobbies.erase(lobby_id);
  }

  void close_all() {
	std::scoped_lock<std::mutex> lock(list_mutex);
	for (auto& [lobby_id, lobby]: lobbies) {
	  lobby.is_closed = true;
	}
  }
};

static bool is_done = false;
static LobbiesList lobbies_list;

void cleanup() {
  lobbies_list.close_all();
  server_socket.close();
  is_done = true;
}

void signalHandler( int signum ) {
  spdlog::info("Shutting down server...");
  cleanup();
  exit(signum);
}

int main() {
  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);
  spdlog::info("Starting Checkers TCP server on port 3000.");
  server_socket.openServerSocket("3000");
  while (!is_done) {
	spdlog::info("Waiting for new connections...");
	Socket player_socket = server_socket.accept();
	spdlog::info("Received new connection from {}", player_socket.getAddressString());
	HandshakeResult handshake_result = receive_handshake(player_socket);
	if (handshake_result.handshake_type == HandshakeType::CREATE_SESSION) {
	  spdlog::info("Player is creating new lobby.");
	  uint32_t lobby_id = lobbies_list.add_lobby(player_socket);
	  if (lobby_id != 0) {
		send_lobby_created(player_socket, lobby_id);
	  } else {
		send_error(player_socket, ErrorType::SERVER_ERROR);
		player_socket.close();
	  }
	} else if (handshake_result.handshake_type == HandshakeType::CONNECT_TO_SESSION) {
	  spdlog::info("Player is connecting to lobby.");
	  if(lobbies_list.add_player_to_lobby(player_socket, handshake_result.lobby_id) == -1) {
		spdlog::warn("Lobby with provided id doesn't exist.");
		send_error(player_socket, ErrorType::LOBBY_NOT_EXISTS);
		player_socket.close();
	  }
	}
  }
  cleanup();
  return 0;
}
