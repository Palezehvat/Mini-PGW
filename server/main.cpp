#include <atomic>
#include <csignal>
#include "udpserver.h"
#include "session.h"
#include "cdr.h"
#include "configmanager.h"
#include "logger.h"
#include "httpserver.h"


std::shared_ptr<std::atomic<bool>> running = std::make_shared<std::atomic<bool>>(true);

void signalHandler(int signal) {
    std::cout << "\nSignal (" << signal << ") received. Stopping server..." << std::endl;
    running->store(false);
}

int main() {
    std::string pathToConfig = "examples/server/configs/config1.json"; // After to args
    std::string pathToLogs = "examples/server/logs/1"; // After to args
    std::string fileLogs = "logs1.txt"; // After to args
    const bool isNeedClearLogFile = true; // After to args
    try {
        auto available = std::make_shared<std::atomic<bool>>(true);
        nLogger::Logger::init("server", pathToLogs, fileLogs, isNeedClearLogFile);
        std::shared_ptr<spdlog::logger> logger = nLogger::Logger::getLogger();
        logger->info("Starting read config for UDP server");

        nConfigManager::ConfigManager configManager(logger);
        configManager.load(pathToConfig);

        nConfigManager::ConfigServer serverConfig 
                        = configManager.get<nConfigManager::ConfigServer>();

        auto cdr = std::make_unique<nCDRManager::CDRManager>(
            logger,
            serverConfig.cdrFile
        );

        auto sessionManager = std::make_shared<nSessionManager::SessionManager>(
            logger,
            std::move(cdr),
            serverConfig.sessionTimeoutSec,
            serverConfig.blacklist
        );
        
        std::unique_ptr<nUdpServer::UdpServer> udpServer = std::make_unique<nUdpServer::UdpServer>(
            serverConfig.udpPort,
            serverConfig.udpIp,
            logger,
            sessionManager,
            running
        );

        std::unique_ptr<nHttpServer::HttpServer> httpServer
        = std::make_unique<nHttpServer::HttpServer>(
            logger,
            running,
            sessionManager,
            serverConfig.httpPort
        );
        
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);

        udpServer->start();
        httpServer->start();

        while (*running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));    
        }

        std::cout << "UDP and HTTP servers will be stop in " << serverConfig.gracefulShutdownRate
                  << " seconds" << std::endl;

        logger->info("The server will not accept any new requests for "
            "{} seconds and will then be stopped", serverConfig.gracefulShutdownRate);

        for (int i = serverConfig.gracefulShutdownRate; i > 0; --i) {
            logger->debug("Graceful shutdown: {} seconds remaining", i);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        httpServer->stop();
        udpServer->stop();
        sessionManager->stopAllSessions();

        return 0;
    } catch (const std::exception& e) {
        std::cout << "There's a problem with the server. Error: " << e.what() << std::endl;
        return 1;
    }

}