#pragma once

#include <string>
#include <netdb.h>


/**
 * @brief The Socket class represents a socket for network communication.
 */
class Socket
{
public:
    /**
     * @brief Default constructor for Socket class.
     */
    Socket() = default;

    /**
     * @brief Constructor for Socket class.
     * @param socketFD The file descriptor of the socket.
     * @param addr The address of the socket.
     */
    Socket(int socketFD, sockaddr_storage addr);

    /**
     * @brief Get the address string of the socket.
     * @return The address string.
     */
    [[nodiscard]] std::string getAddressString() const;

    /**
     * @brief Get the IP string of the socket.
     * @return The IP string.
     */
    [[nodiscard]] std::string getIpString() const;

    /**
     * @brief Get the port string of the socket.
     * @return The port string.
     */
    [[nodiscard]] std::string getPortString() const;

    /**
     * @brief Get the file descriptor of the socket.
     * @return The file descriptor.
     */
    [[nodiscard]] int getSocketFd() const {return socketFD;}

    /**
     * @brief Open a client socket.
     * @param ip The IP address to connect to.
     * @param port The port to connect to.
     */
    void openClientSocket(const char *ip, const char *port);

    /**
     * @brief Open a server socket.
     * @param port The port to listen on.
     */
    void openServerSocket(const char *port);

    /**
     * @brief Constructor for Socket class.
     * @param socketFD The file descriptor of the socket.
     */
    explicit Socket(int socketFD);

    /**
     * @brief Accept a new connection on the socket.
     * @return The new Socket object representing the accepted connection.
     */
    [[nodiscard]] Socket accept() const;

    /**
     * @brief Send data on the socket.
     * @param buf The buffer containing the data to send.
     * @param len The length of the data to send.
     * @return The number of bytes sent.
     */
    size_t sendAll(const char *buf, int len);

    /**
     * @brief Receive data from the socket.
     * @param buf The buffer to store the received data.
     * @param len The length of the buffer.
     * @return The number of bytes received.
     */
    size_t receiveAll(unsigned char *buf, int len) const;

    /**
     * @brief Close the socket.
     */
    void close();

private:
    int socketFD = -1; /**< The file descriptor of the socket. */
    struct sockaddr_storage address; /**< The address of the socket. */
};
