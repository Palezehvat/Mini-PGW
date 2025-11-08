/**
 * @file udpclient.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-11-07
 * 
 * @copyright Copyright (c) 2025
 * 
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
 * @brief 
 * 
 */
namespace nUdpClient {
/**
 * @brief 
 * 
 */
class UdpClient {

public:
    /**
     * @brief Construct a new Udp Client object
     * 
     * @param serverPort 
     * @param serverIp 
     * @param logger 
     */
    UdpClient(const int& serverPort,
              const std::string& serverIp,
              std::shared_ptr<spdlog::logger> logger);
    /**
     * @brief 
     * 
     * @param imsi 
     * @return std::string 
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