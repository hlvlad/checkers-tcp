#pragma once

#include "message.h"
#include "board.h"

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>

/**
 * @brief The NetworkConfig struct represents the network configuration.
 */
struct NetworkConfig {
  QString address; /**< The server address. */
  uint16_t port; /**< The server port. */
};

/**
 * @brief The MessageHandler class handles communication with the server.
 */
class MessageHandler : public QObject
{
    Q_OBJECT

public:
  /**
   * @brief The NetworkSessionStatus enum represents the network session status.
   */
  enum NetworkSessionStatus {
    CONNECTED, /**< Connected to the server. */
    DISCONNECTED /**< Disconnected from the server. */
  };

  /**
   * @brief Constructs a MessageHandler object.
   * @param parent The parent object.
   */
  explicit MessageHandler(QObject *parent = nullptr);

  /**
   * @brief Connects to the server.
   */
  void connect_to_server();

  /**
   * @brief Sends a message to the server.
   * @param message_storage The message to send.
   */
  void send_message(const MessageStorage &message_storage);

  /**
   * @brief Sends a handshake message to the server with a specified lobby ID.
   * @param lobby_id The lobby ID.
   */
  void send_handshake(uint32_t lobby_id);

  /**
   * @brief Sends a handshake message to the server.
   */
  void send_handshake();

  /**
   * @brief Sends a move message to the server.
   * @param move The move to send.
   */
  void send_move(const Move& move);

  /**
   * @brief Sends a resign message to the server.
   */
  void send_resign();

  /**
   * @brief Receives a message from the server.
   * @param message_storage The received message.
   */
  void receive_message(MessageStorage& message_storage);

  /**
   * @brief Gets the connection status.
   * @return The connection status.
   */
  NetworkSessionStatus get_connection_status();

  /**
   * @brief Sets the server address and port.
   * @param address The server address.
   * @param port The server port.
   */
  void set_server_address(QString address, quint16 port);

signals:
  /**
   * @brief Signal emitted when a lobby is created.
   * @param lobby_id The ID of the created lobby.
   */
  void lobbyCreated(quint32 lobby_id);

  /**
   * @brief Signal emitted when a game is started.
   * @param game_flags The flags indicating the game settings.
   */
  void gameStarted(GameFlags game_flags);

  /**
   * @brief Signal emitted when an error occurs.
   * @param error_type The type of the error.
   */
  void errorOccurred(ErrorType error_type);

  /**
   * @brief Signal emitted when a move is received.
   * @param move The received move.
   */
  void moveReceived(Move move);

  /**
   * @brief Signal emitted when a resign message is received.
   */
  void resignReceived();

public slots:
  /**
   * @brief Slot called when a connection error occurs.
   * @param socketError The socket error.
   */
  void connectionError(QTcpSocket::SocketError socketError);

  /**
   * @brief Slot called when a message is received from the server.
   */
  void handle_message();

private:
  NetworkConfig network_config{"localhost", 3000}; /**< The network configuration. */
  QTcpSocket* server_socket = nullptr; /**< The TCP socket for communication with the server. */
};
