/**
 * @file session.h
 * @brief Required for working with sessions
 */
#ifndef SESSION_H
#define SESSION_H

#include <unordered_map>
#include <thread>
#include <atomic>
#include <mutex>
#include "config_manager.h"
#include "logger.h"
#include "cdr.h"

/**
 * @namespace nSessionManager
 * @brief Contains class SessionManager and struct Session
 */
namespace  nSessionManager {

/**
 * @struct Session
 * @brief Is a container that stores the main fields associated with a session
 * 
 * Fields:
 * - Session creation time
 * - IMSI
 */
struct Session {
    std::chrono::steady_clock::time_point startSession;
    std::string imsi;
};

/**
 * @class SessionManager
 * @brief Creates and tracks sessions. If a session expires, it is deleted
 */
class SessionManager {
public:
    /**
     * @brief Constructor
     * @param logger - instrument for logging some successes or failures
     * @param sessionTimeoutSec - The time after which the session should be deleted
     * @param blacklist - blacklist of unwanted IMSIs
     */
    SessionManager(std::shared_ptr<spdlog::logger> logger,
                   std::unique_ptr<nCDRManager::CDRManager> cdr,
                   const int& sessionTimeoutSec,
                   std::vector<std::string> blacklist,
                   const int& gracefulShutdownRate);
    /**
     * @brief Create a new session
     * @param imsi - unique identifier
     * @return true - if session created
     * @return false - another
     */
    bool createSession(const std::string& imsi);
    /**
     * @brief Adds a record to the CDR if an attempt
     * is made to create a session while the server is shut down or imsi is incorrect
     */
    void addRecordForRejectSession(const std::string& imsi);
    /**
     * @brief Checks if there is already a session with the given IMSI
     * @param imsi - unique identifier
     * @return true - if session exist
     * @return false - another
     */
    bool hasSession(const std::string& imsi) const;
    /**
     * @brief Stop all sessions
     */
    void stopAllSessions();
    /**
     * @brief Destroy the Session Manager object
     */
    ~SessionManager();
private:
    std::unique_ptr<nCDRManager::CDRManager> cdr;
    mutable std::mutex mtx;
    std::thread cleanup;
    std::unordered_map<std::string, Session> sessions;
    int sessionTimeoutSec;
    std::vector<std::string> blacklist;
    std::shared_ptr<spdlog::logger> logger;
    std::atomic<bool> running{false};
    int gracefulShutdownRate;

    void cleanupAllSessions();
    void cleanupExpiredSessions();
    void startCleanupThread();
};

} // nSessionManager

#endif // SESSION_H