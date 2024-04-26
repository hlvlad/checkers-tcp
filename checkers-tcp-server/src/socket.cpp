#include "socket.h"

#include <spdlog/spdlog.h>

#include <stdexcept>
#include <cstring>
#include <unistd.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

Socket::Socket(int socketFD) : socketFD(socketFD) {}

Socket::Socket(int socketFD, struct sockaddr_storage addr) : socketFD(socketFD), address(addr) {}

/**
 * @brief Socket::openClientSocket connects client socket to givent address.
 * @param ip address to cennect.
 * @param port port to cennect.
 */
void Socket::openClientSocket(const char *ip, const char *port)
{

  int status, sockfd;
  struct addrinfo hints = {0};
  struct addrinfo *servinfo = nullptr;
  struct addrinfo *p = nullptr;

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((status = getaddrinfo(ip, port, &hints, &servinfo)) != 0)
  {
    throw std::runtime_error("Invalid IP address or port: " + std::string(gai_strerror(status)));
  }

  for (p = servinfo; p != nullptr; p = p->ai_next)
  {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
    {
      spdlog::warn("openServerSocket: Couldn't connect to socket.");
      continue;
    }

    if (::connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
    {
      throw std::runtime_error("Could not connect to client.");
    }

    break;
  }

  if (p == nullptr)
  {
    throw std::runtime_error("Failed to connect to client.");
  }
  address = *((struct sockaddr_storage *)p->ai_addr);

  freeaddrinfo(servinfo);

  if (socketFD != -1)
  {
    if (::close(socketFD) == -1)
    {
      throw std::runtime_error("Failed to close socket.");
    }
  }
  socketFD = sockfd;
}

/**
 * @brief Socket::openServerSocket open listening socket on given port.
 * @param port port number to listen.
 */
void Socket::openServerSocket(const char *port)
{
  int status = -1;
  int sockfd = -1;
  int yes = 1;

  struct addrinfo *servinfo = nullptr;
  struct addrinfo *p = nullptr;
  struct addrinfo hints = {0};

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((status = getaddrinfo(nullptr, port, &hints, &servinfo)) != 0)
  {
    throw std::runtime_error("Invalid IP address or port: " + std::string(gai_strerror(status)));
  }

  for (p = servinfo; p != nullptr; p = p->ai_next)
  {

    if ((sockfd = ::socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
    {
      spdlog::warn("openServerSocket: Couldn't connect to socket.");
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
      throw std::runtime_error("Could not set SO_REUSEADDR.");
    }

    if (::bind(sockfd, p->ai_addr, p->ai_addrlen) < 0)
    {
      ::close(socketFD);
      continue;
    }

    break;
  }

  if (p == nullptr)
  {
    throw std::runtime_error("Failed to bind server socket.");
  }

  address = *((struct sockaddr_storage *)p->ai_addr);

  freeaddrinfo(servinfo);

  if (::listen(sockfd, 64) == -1)
  {
    if (::close(socketFD) == -1)
    {
      throw std::runtime_error("Failed to close socket.");
    }
    throw std::runtime_error("Failed to start listening server socket.");
  }

  if ((socketFD != -1) && (socketFD != sockfd))
  {
    if (::close(socketFD) == -1)
    {
      throw std::runtime_error("Failed to close socket.");
    }
  }
  socketFD = sockfd;
}

/**
 * @brief Socket::getPortString get port number in human readable format.
 * @return port number string.
 */
std::string Socket::getPortString() const
{
  uint16_t port;
  if (address.ss_family == AF_INET)
  {
    port = ((struct sockaddr_in *)&address)->sin_port;
    port = ntohs(port);
  }
  else
  {
    port = ((struct sockaddr_in6 *)&address)->sin6_port;
    port = ntohs(port);
  }
  return std::to_string(port);
}

/**
 * @brief Socket::getsIpString get ip address in human readable format.
 * @return ip address string.
 */
std::string Socket::getIpString() const
{
  char ipstr[INET6_ADDRSTRLEN];
  if (address.ss_family == AF_INET)
  { // IPv4
    struct in_addr ipv4 = ((struct sockaddr_in *)&address)->sin_addr;
    inet_ntop(address.ss_family, &ipv4, ipstr, sizeof ipstr);
  }
  else
  { // IPv6
    struct in6_addr ipv6 = ((struct sockaddr_in6 *)&address)->sin6_addr;
    inet_ntop(address.ss_family, &ipv6, ipstr, sizeof ipstr);
  }
  return {ipstr};
}

/**
 * @brief Socket::getAddressString get full address (ip + port) in human readable format.
 * @return full address string.
 */
std::string Socket::getAddressString() const
{
  return getIpString() + ":" + getPortString();
}

/**
 * @brief Socket::accept accepts new connection.
 * @return socket with new connection.
 */
Socket Socket::accept() const
{
  struct sockaddr_storage their_addr = {0};
  socklen_t len = sizeof their_addr;

  int otherSocket = ::accept(socketFD, (struct sockaddr *)&their_addr, &len);
  if (otherSocket == -1)
  {
    throw std::runtime_error("Failed to accept incoming connection: " + std::string(strerror(errno)));
  }
  return {otherSocket, their_addr};
}

/**
 * @brief Socket::sendAll sends len bytes from buf buffer.
 * @param buf buffer where data is stored.
 * @param len length of data in bytes.
 * @return number of sent bytes.
 */
size_t Socket::sendAll(const char *buf, int len)
{
  if (socketFD == -1)
  {
    spdlog::error("sendAll: Socket is not connected.");
    return 0;
  }


  size_t total = 0; // how many bytes we've sent
  size_t n;

  while (total < len)
  {
    n = send(socketFD, buf + total, len - total, 0);
    if (n == -1)
    {
      throw std::runtime_error("Failed to send data.");
    }
    total += n;
  }

  return total; // return -1 on failure, 0 on success
}

/**
 * @brief Socket::receiveAll receive len bytes into buf buffer.
 * @param buf buffer where received data should be stored.
 * @param len expected num of received bytes in bytes.
 * @return
 */
size_t Socket::receiveAll(unsigned char *buf, int len) const
{
  if (socketFD == -1)
  {
    spdlog::error("receiveAll: Socket is not connected.");
    return 0;
  }

  size_t total = 0;

  while (total < len)
  {
    const size_t n = recv(socketFD, buf + total, len - total, 0);

    if (n == -1)
    {
      throw std::runtime_error("Failed to receive data.");
    }
    total += n;
  }

  return total;
}

/**
 * @brief Socket::close close socket, also shutdown berore to unblock all blocking operations.
 */
void Socket::close()
{
  if (socketFD == -1)
  {
    spdlog::warn("close: Socket is already closed.");
    return;
  }

  int status = ::shutdown(socketFD, SHUT_RDWR);

  if (status == -1)
  {
    throw std::runtime_error("Failed to shutdown socket: " + std::string(strerror(errno)));
  }
  status = ::close(socketFD);
  if (status == -1)
  {
    throw std::runtime_error("Failed to close socket: " + std::string(strerror(errno)));
  }

  socketFD = -1;
}