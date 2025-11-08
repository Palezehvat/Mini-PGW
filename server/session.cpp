#include "session.h"

namespace nSessionManager {

SessionManager::SessionManager(std::shared_ptr<spdlog::logger> logger,
                               std::unique_ptr<nCDRManager::CDRManager> cdr,
                               const int& sessionTimeoutSec,
                               std::vector<std::string> blacklist) :
                               logger(logger), sessionTimeoutSec(sessionTimeoutSec),
                               blacklist(blacklist), cdr(std::move(cdr)) {
    running.store(true);
    startCleanupThread();
    logger->debug("SessionManager initialized");
}

bool SessionManager::createSession(const std::string& imsi) {
    std::lock_guard<std::mutex> lock(mtx);

    if (std::find(blacklist.begin(), blacklist.end(), imsi) != blacklist.end()) {
        logger->warn("IMSI = {} in blacklist", imsi);
        cdr->writeRecord(imsi, "rejected");
        return false;
    }
    
    if (sessions.find(imsi) != sessions.end()) {
        logger->warn("IMSI = {}. Session already existed", imsi);
        cdr->writeRecord(imsi, "rejected");
        return false;
    }

    auto startSession = std::chrono::steady_clock::now();
    Session session{startSession, imsi};

    sessions[imsi] = std::move(session);
    logger->info("IMSI = {}. Session created successfully", imsi);
    cdr->writeRecord(imsi, "created");
    return true;
}

void SessionManager::cleanupExpiredSessions() {
    std::lock_guard<std::mutex> lock(mtx);
    auto now = std::chrono::steady_clock::now();
    size_t removed = 0;

    for (auto it = sessions.begin(); it != sessions.end(); ) {
        auto countTime = std::chrono::duration_cast<std::chrono::seconds>(
            now - it->second.startSession
        ).count();

        if (countTime >= sessionTimeoutSec) {
            logger->info("Removed session with IMSI = {}", it->first);
            cdr->writeRecord(it->first, "removed");
            it = sessions.erase(it);
            ++removed;
        } else {
            ++it;
        }
    }
    if (removed > 0) logger->debug("Removed {} sessions", removed);
}

void SessionManager::cleanupAllSessions() {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto it = sessions.begin(); it != sessions.end(); ++it) {
        cdr->writeRecord(it->first, "removed");
        logger->debug("All sessions are being deleted. Session {} removed", it->first);
    }
    sessions.clear();
    logger->info("All sessions cleared");
}

void SessionManager::startCleanupThread() {
    logger->debug("Run session cleanup every second");
    cleanup = std::thread([this]() {
        while(running) {
            cleanupExpiredSessions();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        cleanupAllSessions();
    });
}

bool SessionManager::hasSession(const std::string& imsi) const {
    std::lock_guard<std::mutex> lock(mtx);
    return sessions.find(imsi) != sessions.end();
}

void SessionManager::stopAllSessions() {
    logger->debug("Stopping all sessions has begun");
    running.store(false);
    if (cleanup.joinable()) {
        cleanup.join();
    }
}

void SessionManager::addRecordForRejectSession(const std::string& imsi) {
    cdr->writeRecord(imsi, "rejected");
}

} // nSessionManager