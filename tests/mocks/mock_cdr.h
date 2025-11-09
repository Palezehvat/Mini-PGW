/**
 * @file mock_cdr.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-11-09
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef MOCKCDR_H
#define MOCKCDR_H

#include <vector>
#include "logger.h"
#include "cdr.h"

/**
 * @brief 
 * 
 */
class MockCDR : public nCDRManager::CDRManager {
public:
    std::vector<std::pair<std::string, std::string>> records;
    
    MockCDR(std::shared_ptr<spdlog::logger> logger) :
    nCDRManager::CDRManager(logger, "/dev/null"){}

    void writeRecord(const std::string& imsi, const std::string& message) {
        records.emplace_back(imsi, message);
    }
};

#endif // MOCKCDR_H