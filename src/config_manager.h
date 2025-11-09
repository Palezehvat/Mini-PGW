/**
 * @file configmanager.h
 * @brief Implements reading of settings specified in the form of a json file
 */
#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <variant>
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include "logger.h"

/**
 * @namespace nConfigManager
 * @brief Contains class ConfigManager and structs: ConfigServer, ConfigClient
 */
namespace nConfigManager {

/**
 * @struct ConfigServer
 * @brief Describes the server settings configuration 
 * 
 * Contains the following fields:
 * - type - has two meanings: (server or Server) and (client or Client)
 *  (for this option (server or Server) should be used) (In your config name parameter: type)
 * - UDP IP (In your config name parameter: udp_ip)
 * - UDP port (In your config name parameter: udp_port)
 * - Session timeout sec (In your config name parameter: session_timeout_sec)
 * - CDR file (In your config name parameter: cdr_file)
 * - HTTP port (In your config name parameter: http_port)
 * - Graceful shutdown rate (In your config name parameter: graceful_shutdown_rate)
 * - Blacklist (In your config name parameter: blacklist)
 */
struct ConfigServer
{
    std::string type;
    std::string udpIp;
    int udpPort;
    int sessionTimeoutSec;
    std::string cdrFile;
    int httpPort;
    int gracefulShutdownRate;
    std::vector<std::string> blacklist;
};

/**
 * @struct ConfigClient
 * @brief Describes the client settings configuration 
 * 
 * Contains the following fields:
 * - type - has two meanings: (server or Server) and (client or Client)
 *  (for this option (client or Client) should be used) (In your config name parameter: type)
 * - Server IP (In your config name parameter: server_ip)
 * - Server port (In your config name parameter: server_port)
 */
struct ConfigClient
{
    std::string type;
    std::string serverIp;
    int serverPort;
};


/**
 * @class ConfigManager
 * @brief Implements reading of settings for the client and server
 */
class ConfigManager {

public:
    // Constructor. Need for get logger
    ConfigManager(std::shared_ptr<spdlog::logger> logger);
    
    /**
     * @brief Reads a json file with settings
     * 
     * @param path - The path where the configuration is located
     */
    void load(const std::string& path);

    /**
     * @brief Getting the required configuration according to the configuration file
     * 
     * @tparam T - Two types of configuration: ConfigServer and ConfigClient
     * @return const T& we return the specified configuration
     */
    template <typename T>
    const T& get() const {
        return std::get<T>(config);
    }

    /**
     * @brief Checks that imsi a string of fifteen digits
     * 
     * @param imsi - string of fifteen digits
     * @return true if imsi a string of fifteen digits
     * @return false another
     */
    static bool checkImsi(const std::string& imsi);

private:
    std::variant<ConfigClient, ConfigServer> config;
    std::shared_ptr<spdlog::logger> logger;

    void createConfigurationForServer(const nlohmann::json& data);
    void createConfigurationForClient(const nlohmann::json& data);

    template <typename FieldType, typename NameStruct>
    void getParameterFromJson(const nlohmann::json& data,
                                             const std::string name,
                                             const std::string nameParameter,
                                             NameStruct& serverOrClient,
                                             FieldType NameStruct::*field) {
        try {
            if (data.contains(nameParameter)) {
                serverOrClient.*field = data[nameParameter].get<FieldType>();
                logger->debug("The {} parameter required for {} configuration has been read",
                               nameParameter, name);
                return;
            }
        } catch (const std::exception& e) {
            logger->critical("Problems with get parameter {} from {} configuration. Error: {}",
                              nameParameter, name, e.what());
            throw std::runtime_error("Problems with get parameter " + nameParameter + " from " 
                                    + name + ". Error: " + e.what());
        }
        logger->critical("The {} parameter required for {} configuration was not found",
                          nameParameter, name);
        throw std::runtime_error("The " + nameParameter + " parameter required for "
                               + name + " configuration was not found");
    }

}; // nConfigManager

}

#endif // CONFIGMANAGER_H