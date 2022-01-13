#ifndef NETWORKSESSION_H
#define NETWORKSESSION_H

#include <QObject>

#include "message_handler.h"


class NetworkSession : public QObject
{
    Q_OBJECT
public:
    explicit NetworkSession(QObject *parent = nullptr);
    Q_INVOKABLE void set_server_address(QString address, QString port);
    Q_INVOKABLE void create_lobby();
    Q_INVOKABLE void connect_lobby(quint32 lobby_id);
    Q_INVOKABLE void send_move(quint8 from, quint8 to, quint8 type);
	Q_INVOKABLE void resign();
 signals:
    void lobbyCreated(quint32 lobby_id);
    void gameStarted(bool is_white);
    void serverErrorOccurred(QString error_message);
    void moveReceived(quint8 from, quint8 to, quint8 type);
    void resignReceived();

public slots:
    void onErrorOccurred(ErrorType error_type);
    void onMoveReceived(Move move);
    void onGameStarted(GameFlags game_flags);
private:
    MessageHandler* network_session = nullptr;
};

#endif // NETWORKSESSION_H
