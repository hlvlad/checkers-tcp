#ifndef NETWORKGAMESESSION_H
#define NETWORKGAMESESSION_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include "message.h"
#include "new_misc.h"

struct NetworkConfig {
  QString address;
  uint16_t port;
};

class NetworkGameSession : public QObject
{
    Q_OBJECT

public:
  enum NetworkSessionStatus {
    CONNECTED,
    DISCONNECTED
  };

  explicit NetworkGameSession(QObject *parent = nullptr);
  void connect_to_server(const QString &address, quint16 port);
  void connect_to_server();
  void send_message(const MessageStorage &message_storage);
  void send_handshake(const uint32_t lobby_id);
  void send_handshake();
  void send_move(const Move& move);
  void receive_message(MessageStorage& message_storage);
  NetworkSessionStatus get_connection_status();
  void wait_for_message();
  void set_server_address(QString address, quint16 port);

signals:
  void lobbyCreated(quint32 lobby_id);
  void gameStarted(GameFlags game_flags);
  void errorOccurred(ErrorType error_type);
  void moveReceived(Move move);

public slots:
  void connectionError(QTcpSocket::SocketError socketError);
  void handle_message();

private:
  NetworkConfig network_config{"localhost", 3000};
  QTcpSocket* server_socket = nullptr;
};

#endif // NETWORKGAMESESSION_H
