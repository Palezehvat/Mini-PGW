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
#include "logger.h"
#include "session.h"
#include "config_manager.h"


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
     * @param port - udp server port from configuration file
     * @param ip - udp server ip from configuration file
     * @param logger - instrument for logging some successes or failures
     * @param sessionManager - responsible for sessions (creation, deletion, logging)
     */
    UdpServer(const int& port, const std::string& ip, std::shared_ptr<spdlog::logger> logger,
              std::shared_ptr<nSessionManager::SessionManager> sessionManager,
              std::shared_ptr<std::atomic<bool>> running);
    /**
     * @brief Runs a UDP server using Berkeley sockets in a separate thread
     */
    void start();
    /**
     * @brief Stops the UDP server
     */
    void stop();
    /**
     * @brief Destroy the Udp Server object
     */
    ~UdpServer();

private:
    int udpSocket;
    sockaddr_in serverAddr{};
    std::shared_ptr<spdlog::logger> logger;
    int port;
    std::string ip;
    std::shared_ptr<std::atomic<bool>> running;
    std::atomic<bool> localRunning;
    std::thread listenerThread;
    std::shared_ptr<nSessionManager::SessionManager> sessionManager;

    void listenLoop();
};

} // nUdpServer

#endif // UDPSERVER_H