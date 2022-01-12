#include "networkgamesession.h"
#include "pack.h"
#include <QDebug>

NetworkGameSession::NetworkGameSession(QObject* parent) : QObject(parent), server_socket(new QTcpSocket(this))
{
    connect(server_socket, &QTcpSocket::errorOccurred, this, &NetworkGameSession::connectionError);
    connect(server_socket, &QTcpSocket::readyRead, this, &NetworkGameSession::handle_message);
}

void NetworkGameSession::connect_to_server() {
  qInfo() << "Connecting to server:" << network_config.address << "/" << network_config.port;
  server_socket->connectToHost(network_config.address, network_config.port);
}
void NetworkGameSession::connectionError(QTcpSocket::SocketError socketError) {
  qCritical() << socketError;
}

void NetworkGameSession::receive_message(MessageStorage &message_storage) {
  char buf[2];
  // Receive message type and length
  int bytes_read = server_socket->read(buf, 2);
  if (bytes_read != 2) {
      qWarning() << "Error when reading message: Bytes read for header:" << bytes_read << "of 2";
  }
  message_storage.message_type = MessageType(buf[0]);
  message_storage.len = buf[1];
  // Receive payload
//  server_socket->waitForReadyRead();
  bytes_read = server_socket->read((char*)message_storage.payload, message_storage.len);
  if (bytes_read != message_storage.len) {
      qWarning() << "Error when reading message: Bytes read for payload:" << bytes_read << "of" << message_storage.len;
  }
  qInfo() << "Received message: " << message_storage;
}


void NetworkGameSession::handle_message() {
  MessageStorage message;
  receive_message(message);
  switch (message.message_type) {
  case LOBBY_CREATED: {
      quint32 lobby_id = unpacku32(message.payload);
      emit lobbyCreated(lobby_id);
      break;
  }
  case GAME_STARTED: {
      GameFlags game_flags = GameFlags(message.payload[0]);
      emit gameStarted(game_flags);
      break;
  }
  case ERROR: {
      ErrorType error_type = ErrorType(message.payload[0]);
      emit errorOccurred(error_type);
      break;
  }
  case MOVE: {
      Move move;
      move.from = SpotIndex(message.payload[0]);
      move.to = SpotIndex(message.payload[1]);
      move.type = MoveType(message.payload[2]);
      emit moveReceived(move);
      break;
  }
  case DISCONNECT: {
      emit errorOccurred(ErrorType::OPPONENT_DISCONNECTED);
      break;
  }
  case HANDSHAKE:
  case RESIGN:
  default:
      break;
  }
}

void NetworkGameSession::send_message(const MessageStorage &message_storage) {
  unsigned char buf[2+MAX_MESSAGE_LEN];
  buf[0] = message_storage.message_type;
  buf[1] = message_storage.len;
  memcpy(&buf[2], message_storage.payload, message_storage.len);
  // Receive message type and length
  server_socket->write((char*)buf, 2+message_storage.len);
  qInfo() << "Send message: " << message_storage;
}

void NetworkGameSession::send_handshake(const uint32_t lobby_id) {
  MessageStorage message_storage{MessageType::HANDSHAKE, 5};
  // Pack handshake type
  message_storage.payload[0] = HandshakeType::CONNECT_TO_SESSION;
  // Pack lobby id
  packi32(&message_storage.payload[1], lobby_id);
  send_message(message_storage);
}

void NetworkGameSession::send_handshake() {
  MessageStorage message_storage{MessageType::HANDSHAKE, 1};
  // Pack handshake type
  message_storage.payload[0] = HandshakeType::CREATE_SESSION;
  send_message(message_storage);
}

void NetworkGameSession::send_move(const Move &move)
{
    MessageStorage message_storage{MessageType::MOVE, 3};
    message_storage.payload[0] = move.from;
    message_storage.payload[1] = move.to;
    message_storage.payload[2] = move.type;
    send_message(message_storage);
}

NetworkGameSession::NetworkSessionStatus NetworkGameSession::get_connection_status() {
  if (server_socket->isOpen()) {
	return NetworkGameSession::CONNECTED;
  } else {
	return NetworkGameSession::DISCONNECTED;
  }
}

