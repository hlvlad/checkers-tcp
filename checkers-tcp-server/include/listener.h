#ifndef LISTENER_H
#define LISTENER_H

#include "socket.h"

#include <QObject>



class Listener : public QObject
{
    Q_OBJECT
public:
    Listener();
    ~Listener();

private:
    Socket listeningSocket;
    QThread *connectionThread;
    QList<QThread*> createdThreads;
    void handleConnection(Socket otherSocket);
    bool isDone;

public slots:
    void startListening(QString port);
    void stopListening();

signals:
    void started(QString port);
    void stopped();
    void connectionRequestReceived(Socket socket);
    void errorOccured(QString errorMessage);
};

#endif // LISTENER_H
