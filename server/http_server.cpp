#include "http_server.h"

namespace nHttpServer {

HttpServer::HttpServer(std::shared_ptr<spdlog::logger> logger,
               std::shared_ptr<std::atomic<bool>> running,
               std::shared_ptr<nSessionManager::SessionManager> sessionManager, 
               const int& port) : logger(logger), sessionManager(sessionManager), 
               port(port), running(running) {
    server = std::make_unique<httplib::Server>();
    logger->debug("HTTP server created on port: {}", port);
}

void HttpServer::start() {
    serverThread = std::thread([this]() {
        server->Post("/stop", [this](const httplib::Request& req, httplib::Response& res) {
            res.set_content("stopping\n", "text/plain");
            logger->warn("HTTP: stop command received");
            *running = false;
        });
        
        server->Get("/check_subscriber", [this](const httplib::Request& req,
                                                httplib::Response& res){
            std::string imsi = req.get_param_value("imsi");
            if (sessionManager->hasSession(imsi)) {
                res.set_content("active\n", "text/plain");
                logger->info("HTTP: check_subscriber IMSI = {}. Result: active", imsi);
            } else {
                res.set_content("not active\n", "text/plain");
                logger->info("HTTP: check_subscriber IMSI = {}. Result: not active", imsi);
            }
        });

        logger->info("HTTP server listening on port = {}", port);
        server->listen("0.0.0.0", port);
        logger->info("HTTP server stopped");
    });
}

void HttpServer::stop() {
    if (server) {
        server->stop();
    }

    if (serverThread.joinable()) {
        serverThread.join();
    }
}

HttpServer::~HttpServer() {
    try {
        logger->debug("Destructor called for HTTP server");

        stop();

        logger->info("HTTP server destroyed cleanly");
    } catch (const std::exception& e) {
        logger->error("Exception in HTTP server destructor. Error: {}", e.what());
    }
}

} // nHttpServer