#ifndef NETWORKGAMESESSION_H
#define NETWORKGAMESESSION_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include "message.h"
#include "../../checkers-tcp-server/include/board.h"

struct NetworkConfig {
  QString address;
  uint16_t port;
};

class MessageHandler : public QObject
{
    Q_OBJECT

public:
  enum NetworkSessionStatus {
    CONNECTED,
    DISCONNECTED
  };

  explicit MessageHandler(QObject *parent = nullptr);
  void connect_to_server();
  void send_message(const MessageStorage &message_storage);
  void send_handshake(uint32_t lobby_id);
  void send_handshake();
  void send_move(const Move& move);
  void send_resign();
  void receive_message(MessageStorage& message_storage);
  NetworkSessionStatus get_connection_status();
  void set_server_address(QString address, quint16 port);

signals:
  void lobbyCreated(quint32 lobby_id);
  void gameStarted(GameFlags game_flags);
  void errorOccurred(ErrorType error_type);
  void moveReceived(Move move);
  void resignReceived();

public slots:
  void connectionError(QTcpSocket::SocketError socketError);
  void handle_message();

private:
  NetworkConfig network_config{"localhost", 3000};
  QTcpSocket* server_socket = nullptr;
};

#endif // NETWORKGAMESESSION_H
