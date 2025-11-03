#include "configmanager.h"

namespace nConfigManager {
    
ConfigManager::ConfigManager(std::shared_ptr<spdlog::logger> logger) : logger(logger) {}

void ConfigManager::createConfigurationForServer(const nlohmann::json& data) {
    ConfigServer server{};

    server.type = "server";
        
    getParameterFromJson(data, "server", "udp_ip", server, &ConfigServer::udpIp);
    getParameterFromJson(data, "server", "udp_port", server, &ConfigServer::udpPort);
    getParameterFromJson(data, "server", "session_timeout_sec", server, 
                        &ConfigServer::sessionTimeoutSec);
    getParameterFromJson(data, "server", "cdr_file", server, &ConfigServer::cdrFile);
    getParameterFromJson(data, "server", "http_port", server, &ConfigServer::httpPort);
    getParameterFromJson(data, "server", "graceful_shutdown_rate", server,
                        &ConfigServer::gracefulShutdownRate);
    getParameterFromJson(data, "server", "blacklist", server, &ConfigServer::blacklist);
    config = server;
}

void ConfigManager::createConfigurationForClient(const nlohmann::json& data) {
    ConfigClient client{};
    client.type = "client";
       
    getParameterFromJson(data, "client", "server_ip",
                         client, &ConfigClient::serverIp);
    getParameterFromJson(data, "client", "server_port",
                         client, &ConfigClient::serverPort);
    config = client;
}

void ConfigManager::load(const std::string& path) {
    logger->debug("The process of reading settings from the config has begun");
    if (!std::filesystem::exists(path)) {
        logger->critical("Unable to find configuration file: {}", path);
        throw std::runtime_error("Unable to find configuration file: " + path);
    }
    logger->debug("Config file found");
    std::ifstream file(path);
    if (!file) {
        logger->critical("Failed to read configuration file: {}", path);
        throw std::runtime_error("Failed to read configuration file: " + path);
    }
    logger->debug("The config file was opened successfully");
    nlohmann::json data;
    try {
        file >> data;
    } catch (const nlohmann::json::parse_error& e) {
        logger->critical("Invalid json format. Error: {}", e.what());
        throw std::runtime_error("Invalid json format. Error: " + std::string(e.what()));
    }
    logger->debug("Config File data is written to a variable");

    if (data.contains("type")) {
        std::string type = data["type"].get<std::string>();
        logger->debug("The file successfully read the type field");
        if (type == "server" || type == "Server") {
            return ConfigManager::createConfigurationForServer(data);
        } else if (type == "client" || type == "Client") {
            return ConfigManager::createConfigurationForClient(data);
        } else {
            logger->critical("The configuration does not meet the specified requirements."
                             "The specified type in the configuration file does not correspond "
                             "to either a client (client/Client) or a server (server/Server)");
            throw std::runtime_error("The configuration does not meet the specified requirements."
                             "The specified type in the configuration file does not correspond "
                             "to either a client (client/Client) or a server (server/Server)");
        }
    }
        
    logger->critical("The configuration does not meet the specified requirements."
                     "The server/client type is not recognized");
    throw std::runtime_error("The configuration does not meet the specified requirements."
                             "The server/client type is not recognized");
}

}