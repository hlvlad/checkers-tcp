#include "networksession.h"


NetworkSession::NetworkSession(QObject *parent) : QObject(parent), network_session(new NetworkGameSession(this))
{
    connect(network_session, &NetworkGameSession::lobbyCreated, this, &NetworkSession::lobbyCreated);
    connect(network_session, &NetworkGameSession::gameStarted, this, &NetworkSession::onGameStarted);
    connect(network_session, &NetworkGameSession::errorOccurred, this, &NetworkSession::onErrorOccurred);
    connect(network_session, &NetworkGameSession::moveReceived, this, &NetworkSession::onMoveReceived);
}

void NetworkSession::set_server_address(QString address, QString port)
{
    network_session->set_server_address(address, port.toUInt());
}

void NetworkSession::create_lobby()
{
    if(network_session->get_connection_status() == NetworkGameSession::DISCONNECTED) {
        network_session->connect_to_server();
        if(network_session->get_connection_status() == NetworkGameSession::DISCONNECTED) {
            return;
        }
    }
    network_session->send_handshake();
}

void NetworkSession::connect_lobby(quint32 lobby_id)
{
    if(network_session->get_connection_status() == NetworkGameSession::DISCONNECTED) {
        network_session->connect_to_server();
        if(network_session->get_connection_status() == NetworkGameSession::DISCONNECTED) {
            return;
        }
    }
    network_session->send_handshake(lobby_id);
}

void NetworkSession::send_move(quint8 from, quint8 to, quint8 type) {
    if(network_session->get_connection_status() == NetworkGameSession::DISCONNECTED) {
        network_session->connect_to_server();
        if(network_session->get_connection_status() == NetworkGameSession::DISCONNECTED) {
            return;
        }
    }
    Move move{SpotIndex(from), SpotIndex(to), MoveType(type)};
    network_session->send_move(move);
}

void NetworkSession::onErrorOccurred(ErrorType error_type)
{
    switch(error_type) {
    case ErrorType::LOBBY_NOT_EXISTS: {
        QString message_text = "Lobby with given id doesn't exist.";
        emit serverErrorOccurred(message_text);
        break;
    }
    case ErrorType::OPPONENT_DISCONNECTED: {
        QString message_text = "Opponent disconnected from game.";
        emit serverErrorOccurred(message_text);
        break;
    }
    case ErrorType::SERVER_DISCONNECTED: {
        QString message_text = "Server closed connection.";
        emit serverErrorOccurred(message_text);
        break;
    }
    case ErrorType::SERVER_ERROR: {
        QString message_text = "Unknown server error occurred.";
        emit serverErrorOccurred(message_text);
        break;
    }
    default:
        // should never reach here
        break;
    }
}

void NetworkSession::onMoveReceived(Move move)
{
    emit moveReceived(move.from, move.to, move.type);
}

void NetworkSession::onGameStarted(GameFlags game_flags)
{
    const bool is_white = game_flags & GameFlags::IM_WHITE;
    emit gameStarted(is_white);
}
