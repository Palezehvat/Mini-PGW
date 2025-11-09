/**
 * @file udpclient.h
 * @brief UDP server client
 */
#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include "logger.h"

/**
 * @namespace nUdpClient 
 * @brief Contains class UdpClient
 */
namespace nUdpClient {
/**
 * @class UdpClient 
 * @brief Implements UDP client functionality (sending IMSI to the UDP server to create a session)
 */
class UdpClient {

public:
    /**
     * @brief Constructor
     * @param serverPort - udp server port from configuration file
     * @param serverIp - udp server ip from configuration file
     * @param logger - instrument for logging some successes or failures
     */
    UdpClient(const int& serverPort,
              const std::string& serverIp,
              std::shared_ptr<spdlog::logger> logger);
    /**
     * @brief Sending to UDP server IMSI for create session
     * 
     * @param imsi - string out of fifteen didgets
     * @return std::string - two answer options (rejected/created)
     */
    std::string sendMessage(const std::string& imsi);
    /**
     * @brief Destroy the Udp Client object
     */
    ~UdpClient();

private:
    sockaddr_in serverAddr{};
    int udpSocket;
    int serverPort;
    std::string serverIp;
    std::shared_ptr<spdlog::logger> logger;

};

}

#endif // UDPCLIENT_H