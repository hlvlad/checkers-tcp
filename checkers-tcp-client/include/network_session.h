#pragma once

#include "message_handler.h"

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>

/**
 * @brief The NetworkSession class represents a network session for a checkers game.
 * 
 * This class handles the communication between the client and the server.
 * It provides methods to set the server address, create a lobby, connect to a lobby,
 * send a move, and resign from the game.
 */
class NetworkSession : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(NetworkSession)


public:
    
    /**
     * @brief Returns a QML instance of the network session.
     *
     * This function is used to create a QML instance of the network session. It takes a QQmlEngine
     * and a QJSEngine as parameters and returns a QObject pointer. The QML instance can be used to
     * interact with the network session in QML code.
     *
     * @param engine The QQmlEngine used to create the QML instance.
     * @param scriptEngine The QJSEngine used to create the QML instance.
     * @return A QObject pointer to the QML instance of the network session.
     */
    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
    {
        Q_UNUSED(engine);
        Q_UNUSED(scriptEngine);

        return new NetworkSession;
    }

    /**
     * @brief Constructs a NetworkSession object.
     * 
     * @param parent The parent object.
     */
    explicit NetworkSession(QObject *parent = nullptr);

    /**
     * @brief Sets the server address and port.
     * 
     * @param address The server address.
     * @param port The server port.
     */
    Q_INVOKABLE void set_server_address(QString address, QString port);

    /**
     * @brief Creates a lobby.
     */
    Q_INVOKABLE void create_lobby();

    /**
     * @brief Connects to a lobby with the specified ID.
     * 
     * @param lobby_id The ID of the lobby to connect to.
     */
    Q_INVOKABLE void connect_lobby(quint32 lobby_id);

    /**
     * @brief Sends a move to the server.
     * 
     * @param from The starting position of the move.
     * @param to The destination position of the move.
     * @param type The type of the move.
     */
    Q_INVOKABLE void send_move(quint8 from, quint8 to, quint8 type);

    /**
     * @brief Resigns from the game.
     */
	Q_INVOKABLE void resign();

signals:
    /**
     * @brief Signal emitted when a lobby is created.
     * 
     * @param lobby_id The ID of the created lobby.
     */
    void lobbyCreated(quint32 lobby_id);

    /**
     * @brief Signal emitted when the game starts.
     * 
     * @param is_white Indicates whether the player is playing as white.
     */
    void gameStarted(bool is_white);

    /**
     * @brief Signal emitted when a server error occurs.
     * 
     * @param error_message The error message.
     */
    void serverErrorOccurred(QString error_message);

    /**
     * @brief Signal emitted when a move is received from the server.
     * 
     * @param from The starting position of the move.
     * @param to The destination position of the move.
     * @param type The type of the move.
     */
    void moveReceived(quint8 from, quint8 to, quint8 type);

    /**
     * @brief Signal emitted when a resign request is received from the server.
     */
    void resignReceived();

public slots:
    /**
     * @brief Slot called when an error occurs.
     * 
     * @param error_type The type of the error.
     */
    void onErrorOccurred(ErrorType error_type);

    /**
     * @brief Slot called when a move is received from the server.
     * 
     * @param move The received move.
     */
    void onMoveReceived(Move move);

    /**
     * @brief Slot called when the game starts.
     * 
     * @param game_flags The flags indicating the game settings.
     */
    void onGameStarted(GameFlags game_flags);

private:
    MessageHandler* network_session = nullptr; /**< The message handler for the network session. */
};
