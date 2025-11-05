/**
 * @file cdr.h
 * @brief Credited for CDR logic
 */
#ifndef CDR_H
#define CDR_H

#include <mutex>
#include <fstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include "logger.h"

/**
 * @namespace nCDRManager 
 * @brief Contains class CDRManager
 */
namespace nCDRManager {
/**
 * @class CDRManager
 * @brief Responsible for the session log
 */
class CDRManager {
public:
    /**
     * @brief Construct a new CDRManager object
     * @param logger - instrument for logging some successes or failures
     * @param logFile - the file where the data will be written
     */
    CDRManager(std::shared_ptr<spdlog::logger> logger, const std::string& cdrPath);
    /**
     * @brief Creates an entry to a file
     * @param imsi - unique identifier
     * @param message - A message that reflects the state of the session
     * Message options:
     * - rejected
     * - created
     * - removed
     */
    void writeRecord(const std::string& imsi, const std::string& message);
    /**
     * @brief Destroy the CDRManager object
     */
    ~CDRManager();
private:
    std::shared_ptr<spdlog::logger> logger;
    std::string cdrPath;
    std::mutex mtx;
    std::ofstream file;

};

} // nCDRManager

#endif // CDR_H