/**
 * @file udpserver.h
 * @brief Required for running a UDP server
 */
#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <thread>
#include <atomic>
#include "configmanager.h"
#include "logger.h"
#include "session.h"
#include "cdr.h"

/**
 * @namespace nUdpServer
 * @brief UdpServer contains class UdpServer
 */
namespace nUdpServer {
/**
 * @class UdpServer
 * @brief Responsible for starting and stopping the UDP server and for reading UDP client requests
 */
class UdpServer {
public:
    /**
     * @brief Construct a new Udp Server object
     * 
     * @param config - configuration for udp server
     * @param logger - instrument for logging some successes or failures
     */
    UdpServer(const nConfigManager::ConfigServer& config, std::shared_ptr<spdlog::logger> logger);
    /**
     * @brief Runs a UDP server using Berkeley sockets in a separate thread
     */
    void start();
    /**
     * @brief Stops the UDP server
     */
    void stop();

private:
    int udpSocket;
    sockaddr_in serverAddr{};
    std::shared_ptr<spdlog::logger> logger;
    nConfigManager::ConfigServer config;
    std::atomic<bool> running{false};
    std::thread listenerThread;
    std::shared_ptr<nSessionManager::SessionManager> sessionManager;
    std::shared_ptr<nCDRManager::CDRManager> cdr;

    void listenLoop();
};

} // nUdpServer

#endif // UDPSERVER_H