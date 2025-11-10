#include "udp_client.h"
#include "config_manager.h"


int main(int argc, char* argv[]) {
    try {
        bool isNeedClearLogFile;
        std::string pathToLogs;
        std::string pathToConfig;
        std::string imsi;

        if (argc == 4 || argc == 5) {
            pathToLogs = argv[1];
            pathToConfig = argv[2];
            imsi = argv[3];
            if (!nConfigManager::ConfigManager::checkImsi(imsi)) {
                throw std::runtime_error("The imsi parameter is incorrect. It must be "
                    "15 digits long. The imsi value is currently: " + imsi);
            }
            if (argc == 5) {
                if (std::string(argv[4]) == "clear_logs") {
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
                                     + std::to_string(argc) + " arguments");
        }

        std::cout << "\nThe following arguments were passed: \n"
                  << "Path logs: " << pathToLogs << "\n"
                  << "Path config:" << pathToConfig << "\n"\
                  << "IMSI:" << imsi << "\n"
                  << "Clear logs: " << std::boolalpha << isNeedClearLogFile << "\n\n";
        nLogger::Logger::init("client", pathToLogs, isNeedClearLogFile);
        std::shared_ptr<spdlog::logger> logger = nLogger::Logger::getLogger();
        
        logger->info("Starting read config for UDP client");


        nConfigManager::ConfigManager configManager(logger);
        configManager.load(pathToConfig);

        nConfigManager::ConfigClient clientConfig 
                        = configManager.get<nConfigManager::ConfigClient>();

        std::unique_ptr<nUdpClient::UdpClient> udpServer = std::make_unique<nUdpClient::UdpClient>(
            clientConfig.serverPort,
            clientConfig.serverIp,
            logger
        );

        std::string response = udpServer->sendMessage(imsi);

        std::cout << "Message get successfully: " << response << std::endl;

        return 0;
    } catch (const std::exception& e) {
        std::cout << "There's a problem with the client. Error: " << e.what() << std::endl;
        return 1;
    }
}