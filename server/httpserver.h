/**
 * @file httpserver.h
 * @brief Responsible for the HTTP server
 */
#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <httplib.h>
#include <atomic>
#include <thread>
#include "session.h"
#include "logger.h"

/**
 * @namespace nHttpServer
 * @brief Contains class HttpServer
 */
namespace nHttpServer {
/**
 * @class HttpServer
 * @brief Implements an HTTP server that responds to a Get request to obtain session status,
 * as well as to a Post request to stop the server
 */
class HttpServer {
public:
    /**
     * @brief Constructor
     * 
     * @param logger - instrument for logging some successes or failures
     * @param sessionManager - responsible for sessions (creation, deletion, logging)
     * @param port - HTTP server port (taken from the config)
     */
    HttpServer(std::shared_ptr<spdlog::logger> logger,
               std::shared_ptr<std::atomic<bool>> running,
               std::shared_ptr<nSessionManager::SessionManager> sessionManager, 
               const int& port);
    /**
     * @brief Running an HTTP server in a separate thread
     */
    void start();
    /**
     * @brief Stops the server in a separate thread
     */
    void stop();
private:

private:
    int port;
    std::shared_ptr<nSessionManager::SessionManager> sessionManager;
    std::shared_ptr<spdlog::logger> logger;
    std::unique_ptr<httplib::Server> server;
    std::shared_ptr<std::atomic<bool>> running;
    std::thread serverThread;
};

} // nHttpServer

#endif // HTTPSERVER_H