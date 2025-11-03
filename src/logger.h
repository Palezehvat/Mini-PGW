/**
 * @file logger.h
 * @brief Contains the implementation of project logging
 */
#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <memory>

/**
 * @namespace nLogger
 * @brief Contains Logger
 */
namespace nLogger {

/**
 * @class Logger
 * @brief Need for logging
 * 
 * Includes:
 * - logging for both server and client
 * - logging of sending, response, and errors for the client
 * - logging of all key actions for the server
 * - logging of various levels:
 * - debug
 * - info
 * - warn
 * - critical
 * - error
 */
class Logger {

public:
    /**
     * @brief Initializing the logger at program startup
     * @param nameLogger - internal name of the logger
     * @param pathDir - path to the folder where the log file will be located
     * @param fileForLogs - log file name
     */
    static void init(const std::string& nameLogger, const std::string& pathDir,
                     const std::string& fileForLogs, const bool& isNeedCleanLogFile = false);
    /**
     * @brief Get the Logger object
     * @return std::shared_ptr<spdlog::logger> 
     */
    static std::shared_ptr<spdlog::logger> getLogger();

private:
    static std::shared_ptr<spdlog::logger> logger;

};

} // nLogger 

#endif // LOGGER_H