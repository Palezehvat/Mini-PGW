#include <atomic>
#include <csignal>
#include "udpserver.h"
#include "configmanager.h"
#include "logger.h"


std::atomic<bool> stopFlag = false;

void signalHandler(int signal) {
    std::cout << "\nSignal (" << signal << ") received. Stopping server..." << std::endl;
    stopFlag = true;
}

int main() {
    std::string pathToConfig = "examples/server/configs/config1.json"; // After to args
    std::string pathToLogs = "examples/server/logs/1"; // After to args
    std::string fileLogs = "logs1.txt"; // After to args
    const bool isNeedClearLogFile = true; // After to args
    try {
        nLogger::Logger::init("server", pathToLogs, fileLogs, isNeedClearLogFile);
        std::shared_ptr<spdlog::logger> logger = nLogger::Logger::getLogger();
        logger->info("Starting read config for UDP server");

        nConfigManager::ConfigManager configManager(logger);
        configManager.load(pathToConfig);

        nConfigManager::ConfigServer serverConfig 
                        = configManager.get<nConfigManager::ConfigServer>();
        
        nUdpServer::UdpServer server(serverConfig, logger);
        
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);

        server.start();

        while (!stopFlag) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));    
        }

        logger->info("Start stopping UDP server");

        server.stop();
        return 0;
    } catch (const std::exception& e) {
        std::cout << "There's a problem with the server. Error: " << e.what() << std::endl;
        return 1;
    }

}