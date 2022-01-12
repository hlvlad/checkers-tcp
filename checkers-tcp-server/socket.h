#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <netdb.h>


class Socket
{
public:
    Socket() = default;

    Socket(int socketFD, sockaddr_storage addr);

    [[nodiscard]] std::string getAddressString() const;
    [[nodiscard]] std::string getIpString() const;
    [[nodiscard]] std::string getPortString() const;
    [[nodiscard]] int getSocketFd() const {return socketFD;}

    void openClientSocket(const char *ip, const char *port);
    void openServerSocket(const char *port);

    explicit Socket(int socketFD);
    [[nodiscard]] Socket accept() const;
    size_t sendAll(const char *buf, int len);
    size_t receiveAll(unsigned char *buf, int len) const;
    void close();
private:
    int socketFD = -1;
    struct sockaddr_storage address{};
};

#endif // SOCKET_H
