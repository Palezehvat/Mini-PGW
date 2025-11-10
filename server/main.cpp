#include <atomic>
#include <csignal>
#include "udp_server.h"
#include "session.h"
#include "cdr.h"
#include "config_manager.h"
#include "logger.h"
#include "http_server.h"


std::shared_ptr<std::atomic<bool>> running = std::make_shared<std::atomic<bool>>(true);

void signalHandler(int signal) {
    std::cout << "\nSignal (" << signal << ") received. Stopping server..." << std::endl;
    running->store(false);
}

bool checkImsi(const std::string& imsi) {
    if (imsi.size() != 15) return false;
    for (size_t i = 0; i < imsi.size(); ++i) {
        if (!isdigit(imsi[i])) return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    try {
        bool isNeedClearLogFile;
        std::string pathToLogs;
        std::string pathToConfig;

        if (argc == 3 || argc == 4) {
            pathToLogs = argv[1];
            pathToConfig = argv[2];
            if (argc == 4) {
                if (std::string(argv[3]) == "clear_logs") {
                    isNeedClearLogFile = true;
                } else {
                    std::cout << "The log clear argument doesn't match single value(clear_logs). "
                        "Therefore, the false parameter is set for this argument." << std::endl;
                    isNeedClearLogFile = false;
                }
            } else {
                isNeedClearLogFile = false;
            }
        } else {
            throw std::runtime_error("The number of arguments does not meet the requirements. "
                                     "There must be 3 or 4 arguments. Currently set: "
                                     + std::to_string(argc) + "arguments");
        }
        std::cout << "\nThe following arguments were passed: \n"
                  << "Path logs: " << pathToLogs << "\n"
                  << "Path config:" << pathToConfig << "\n"
                  << "Clear logs: " << std::boolalpha << isNeedClearLogFile << "\n\n";

        nLogger::Logger::init("server", pathToLogs, isNeedClearLogFile);
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
        if (serverConfig.gracefulShutdownRate <= 0) {
            logger->error("Invalid number of sessions specified during cleanup after shutdown: {}."
                          "This value will be changed to 10", serverConfig.gracefulShutdownRate);
            serverConfig.gracefulShutdownRate = 10;
        }
        auto sessionManager = std::make_shared<nSessionManager::SessionManager>(
            logger,
            std::move(cdr),
            serverConfig.sessionTimeoutSec,
            serverConfig.blacklist,
            serverConfig.gracefulShutdownRate
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

        std::cout << "Server has started its work" << std::endl;

        while (*running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));    
        }

        logger->info("The server will not accept any new requests and will then be stopped");
        std::cout << "The server will not accept any new requests and will then be stopped"
                  << std::endl;

        httpServer->stop();
        udpServer->stop();
        sessionManager->stopAllSessions();

        return 0;
    } catch (const std::exception& e) {
        std::cout << "There's a problem with the server. Error: " << e.what() << std::endl;
        return 1;
    }

}