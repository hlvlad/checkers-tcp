/**
 * @file network_session.cpp
 * @brief Contains the implementation of the NetworkSession class.
 */

#include "network_session.h"

/**
 * @brief Constructs a NetworkSession object.
 * @param parent The parent QObject.
 */
NetworkSession::NetworkSession(QObject *parent) : QObject(parent), network_session(new MessageHandler(this))
{
    connect(network_session, &MessageHandler::lobbyCreated, this, &NetworkSession::lobbyCreated);
    connect(network_session, &MessageHandler::gameStarted, this, &NetworkSession::onGameStarted);
    connect(network_session, &MessageHandler::errorOccurred, this, &NetworkSession::onErrorOccurred);
    connect(network_session, &MessageHandler::moveReceived, this, &NetworkSession::onMoveReceived);
    connect(network_session, &MessageHandler::resignReceived, this, &NetworkSession::resignReceived);
}

/**
 * @brief Sets the server address and port.
 * @param address The server address.
 * @param port The server port.
 */
void NetworkSession::set_server_address(QString address, QString port)
{
    network_session->set_server_address(address, port.toUInt());
}

/**
 * @brief Creates a lobby.
 * If the network session is disconnected, it connects to the server and sends a handshake.
 * If the connection fails, it returns without creating a lobby.
 */
void NetworkSession::create_lobby()
{
    if(network_session->get_connection_status() == MessageHandler::DISCONNECTED) {
        network_session->connect_to_server();
        if(network_session->get_connection_status() == MessageHandler::DISCONNECTED) {
            return;
        }
    }
    network_session->send_handshake();
}

/**
 * @brief Connects to a lobby with the given lobby ID.
 * If the network session is disconnected, it connects to the server and sends a handshake with the lobby ID.
 * If the connection fails, it returns without connecting to the lobby.
 * @param lobby_id The ID of the lobby to connect to.
 */
void NetworkSession::connect_lobby(quint32 lobby_id)
{
    if(network_session->get_connection_status() == MessageHandler::DISCONNECTED) {
        network_session->connect_to_server();
        if(network_session->get_connection_status() == MessageHandler::DISCONNECTED) {
            return;
        }
    }
    network_session->send_handshake(lobby_id);
}

/**
 * @brief Sends a move to the server.
 * If the network session is disconnected, it connects to the server and sends the move.
 * If the connection fails, it returns without sending the move.
 * @param from The index of the spot where the move starts.
 * @param to The index of the spot where the move ends.
 * @param type The type of the move.
 */
void NetworkSession::send_move(quint8 from, quint8 to, quint8 type) {
    if(network_session->get_connection_status() == MessageHandler::DISCONNECTED) {
        network_session->connect_to_server();
        if(network_session->get_connection_status() == MessageHandler::DISCONNECTED) {
            return;
        }
    }
    Move move{SpotIndex(from), SpotIndex(to), MoveType(type)};
    network_session->send_move(move);
}

/**
 * @brief Sends a resignation signal to the server.
 */
void NetworkSession::resign()
{
  network_session->send_resign();
}

/**
 * @brief Handles the error occurred signal from the network session.
 * Emits a serverErrorOccurred signal with an appropriate error message based on the error type.
 * @param error_type The type of the error occurred.
 */
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

/**
 * @brief Handles the move received signal from the network session.
 * Emits a moveReceived signal with the move details.
 * @param move The move received.
 */
void NetworkSession::onMoveReceived(Move move)
{
    emit moveReceived(move.from, move.to, move.type);
}

/**
 * @brief Handles the game started signal from the network session.
 * Emits a gameStarted signal with the information whether the player is white or not.
 * @param game_flags The game flags indicating the player's color.
 */
void NetworkSession::onGameStarted(GameFlags game_flags)
{
    const bool is_white = game_flags & GameFlags::IM_WHITE;
    emit gameStarted(is_white);
}
