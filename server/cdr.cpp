#include "cdr.h"

namespace nCDRManager {

CDRManager::CDRManager(std::shared_ptr<spdlog::logger> logger, const std::string& cdrPath)
                     : logger(logger), cdrPath(cdrPath) {
    file = std::ofstream(cdrPath, std::ios::app);
    if (!file.is_open()) {
        logger->error("CDR file not opened");
        throw std::runtime_error("CDR file not opened");
    }
    logger->info("CDR manager started successfully with file: {}", cdrPath);
}

void CDRManager::writeRecord(const std::string& imsi, const std::string& action) {
    std::lock_guard<std::mutex> lock(mtx);
    auto now = std::chrono::system_clock::now();
    if (file.is_open()) {
        file << std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count()
             << ", " << imsi
             << ", " << action << "\n";
        file.flush();
        logger->info("New record for cdr file. IMSI = {} was {}", imsi, action);
        return;
    }
    logger->error("CDR file not opened for write: {}, {}, {}",
        std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count(),
        imsi, action);
}

CDRManager::~CDRManager() {
    std::lock_guard<std::mutex> lock(mtx);
    if (file.is_open()) {
        file.close();
    }
}

} // nCDRManager