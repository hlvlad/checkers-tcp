#include "gameprovider.h"
#include "listener.h"

#include <QCoreApplication>
#include <QThread>

Listener::Listener() : isDone(false) { }

/**
 * @brief Listener::~Listener interrupt all accepted connections before destroying.
 */
Listener::~Listener()
{
    foreach(QThread *thread, createdThreads) {
         if(thread != nullptr) {
             thread->quit();
             thread->wait();
         }
    }
    listeningSocket.close();
}

/**
 * @brief Listener::startListening start listening on given port.
 * @param port port on which Listener should listen.
 */
void Listener::startListening(QString port) {
    try {
        isDone = false;
        listeningSocket.openServerSocket(port.toLocal8Bit().data());
        emit started(listeningSocket.getPortString());

        while(!isDone) {
            Socket otherSocket = listeningSocket.accept();
            emit connectionRequestReceived(otherSocket);
            QCoreApplication::processEvents();
        }
        listeningSocket.close();
        emit stopped();

    }  catch (std::exception e) {
        emit errorOccured(QString::fromStdString(e.what()));
        return;
    }
}


/**
 * @brief Listener::stopListening stop listening loop (listening has to be unblocked via connection from Network).
 */
void Listener::stopListening() {
    isDone = true;
}
