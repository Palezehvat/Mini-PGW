#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/null_sink.h>
#include "logger.h"
#include "config_manager.h"
#include "session.h"
#include "cdr.h"
#include "udp_server.h"
#include "http_server.h"
#include "mock_cdr.h"

TEST(cdr, isInInitializationFileForCDRCreates) {
    std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy15");
    std::string path = "./tests/very_important_cdr.txt";
    if (std::filesystem::exists(path))
        std::filesystem::remove(path);
    EXPECT_NO_THROW(std::shared_ptr<nCDRManager::CDRManager> cdr 
        = std::make_shared<nCDRManager::CDRManager>(dummyLogger, path););
    if (std::filesystem::exists(path))
        std::filesystem::remove(path);
}

TEST(cdr, isWriteRecordAppendsLineInCDR) {
    std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy16");
    std::string path = "./tests/very_important_cdr.txt";

    if (std::filesystem::exists(path))
        std::filesystem::remove(path);
    std::shared_ptr<nCDRManager::CDRManager> cdr;

    EXPECT_NO_THROW(cdr = std::make_shared<nCDRManager::CDRManager>(dummyLogger, path););

    cdr->writeRecord("123456789012346", "created");
    cdr->writeRecord("123456789012347", "created");
    cdr.reset();

    std::ifstream ifs(path);
    
    EXPECT_TRUE(ifs.is_open());

    std::vector<std::string> lines;
    std::string line;

    while(getline(ifs, line)) {
        lines.push_back(line);
    }
    ifs.close();

    EXPECT_EQ(lines.size(), 2);
    EXPECT_NE(lines[0].find("123456789012346"), std::string::npos);
    EXPECT_NE(lines[0].find("created"), std::string::npos);
    EXPECT_NE(lines[1].find("123456789012347"), std::string::npos);
    EXPECT_NE(lines[1].find("created"), std::string::npos);

    if (std::filesystem::exists(path))
        std::filesystem::remove(path);
}

TEST(cdr, isThrownExceptionIfCannotOpenFile) {
    std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy17");
    std::string path = "./tests/tests/very_important_cdr.txt";
    if (std::filesystem::exists(path))
        std::filesystem::remove(path);
    std::shared_ptr<nCDRManager::CDRManager> cdr;
    EXPECT_THROW(
        cdr = std::make_shared<nCDRManager::CDRManager>(dummyLogger, path),
        std::runtime_error
    );
}

TEST(sessions, isInitializeSessionManagerCorrect) {
    std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy18");
    std::shared_ptr<nSessionManager::SessionManager> sessions;
    auto cdr = std::make_unique<MockCDR>(dummyLogger);
    EXPECT_NO_THROW(
        sessions = std::make_shared<nSessionManager::SessionManager>(
            dummyLogger,
            std::move(cdr),
            10,
            std::vector<std::string>({"123456789012345"})
        );
        sessions->stopAllSessions();
    );
}

TEST(sessions, isCreateSessionSuccess) {
    std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy19");
    std::shared_ptr<nSessionManager::SessionManager> sessions;
    auto cdr = std::make_unique<MockCDR>(dummyLogger);
    sessions = std::make_shared<nSessionManager::SessionManager>(
        dummyLogger,
        std::move(cdr),
        10,
        std::vector<std::string>({"123456789012345"})
    );
    EXPECT_TRUE(sessions->createSession("123456789012346"));
    EXPECT_TRUE(sessions->hasSession("123456789012346"));
    EXPECT_NO_THROW(
        sessions->stopAllSessions();
    );
}

TEST(sessions, isCreateSessionDuplicateRejected) {
    std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy20");
    std::shared_ptr<nSessionManager::SessionManager> sessions;
    auto cdr = std::make_unique<MockCDR>(dummyLogger);
    sessions = std::make_shared<nSessionManager::SessionManager>(
        dummyLogger,
        std::move(cdr),
        10,
        std::vector<std::string>({"123456789012345"})
    );
    EXPECT_TRUE(sessions->createSession("123456789012346"));
    EXPECT_FALSE(sessions->createSession("123456789012346"));
    EXPECT_TRUE(sessions->hasSession("123456789012346"));
    EXPECT_NO_THROW(
        sessions->stopAllSessions();
    );
}

TEST(sessions, isSessionRejectedIfItInBlacklist) {
    std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy21");
    std::shared_ptr<nSessionManager::SessionManager> sessions;
    auto cdr = std::make_unique<MockCDR>(dummyLogger);
    sessions = std::make_shared<nSessionManager::SessionManager>(
        dummyLogger,
        std::move(cdr),
        10,
        std::vector<std::string>({"123456789012345"})
    );
    EXPECT_FALSE(sessions->createSession("123456789012345"));
    EXPECT_FALSE(sessions->hasSession("123456789012345"));
    EXPECT_NO_THROW(
        sessions->stopAllSessions();
    );
}

TEST(sessions, isSessionsCleanupAfterExpired) {
    std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy22");
    std::shared_ptr<nSessionManager::SessionManager> sessions;
    auto cdr = std::make_unique<MockCDR>(dummyLogger);
    sessions = std::make_shared<nSessionManager::SessionManager>(
        dummyLogger,
        std::move(cdr),
        1,
        std::vector<std::string>({"123456789012345"})
    );
    EXPECT_TRUE(sessions->createSession("123456789012346"));
    std::this_thread::sleep_for(std::chrono::seconds(3));
    EXPECT_FALSE(sessions->hasSession("123456789012346"));
    EXPECT_NO_THROW(
        sessions->stopAllSessions();
    );
}

TEST(sessions, isStopAllSessionsAfterStopSessionManager) {
    std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy23");
    std::shared_ptr<nSessionManager::SessionManager> sessions;
    auto cdr = std::make_unique<MockCDR>(dummyLogger);
    sessions = std::make_shared<nSessionManager::SessionManager>(
        dummyLogger,
        std::move(cdr),
        8,
        std::vector<std::string>({"123456789012345"})
    );
    EXPECT_TRUE(sessions->createSession("123456789012346"));
    sessions->stopAllSessions();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    EXPECT_FALSE(sessions->hasSession("123456789012346"));
    EXPECT_NO_THROW(
        sessions->stopAllSessions();
    );
}

TEST(udpServer, isInitializationUDPServerCorrect) {
    std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy24");
    auto cdr = std::make_unique<MockCDR>(dummyLogger);
    auto sessions = std::make_shared<nSessionManager::SessionManager>(
        dummyLogger,
        std::move(cdr),
        5,
        std::vector<std::string>({"123456789012345"})
    );
    std::shared_ptr<nUdpServer::UdpServer> udpServer;
    std::shared_ptr<std::atomic<bool>> running = std::make_shared<std::atomic<bool>>(true);
    EXPECT_NO_THROW(
        udpServer = std::make_shared<nUdpServer::UdpServer>(
            9090,
            "127.0.0.1",
            dummyLogger,
            sessions,
            running
        )
    );
    EXPECT_NO_THROW(
        udpServer->stop();
        sessions->stopAllSessions();
    );
}

TEST(udpServer, isInitializationUDPServerIncorrectWithWrongParams) {
    std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy25");
    auto cdr = std::make_unique<MockCDR>(dummyLogger);
    auto sessions = std::make_shared<nSessionManager::SessionManager>(
        dummyLogger,
        std::move(cdr),
        5,
        std::vector<std::string>({"123456789012345"})
    );
    std::shared_ptr<nUdpServer::UdpServer> udpServer;
    std::shared_ptr<std::atomic<bool>> running = std::make_shared<std::atomic<bool>>(true);
    EXPECT_THROW(
        udpServer = std::make_shared<nUdpServer::UdpServer>(
            9091,
            "9999.9999.9999.9999",
            dummyLogger,
            sessions,
            running
        ), 
        std::runtime_error
    );
    EXPECT_NO_THROW(
        sessions->stopAllSessions();
    );
}

TEST(httpServer, isInitializationHTTPServerCorrect) {
    std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy26");
    auto cdr = std::make_unique<MockCDR>(dummyLogger);
    auto sessions = std::make_shared<nSessionManager::SessionManager>(
        dummyLogger,
        std::move(cdr),
        5,
        std::vector<std::string>({"123456789012345"})
    );
    std::shared_ptr<std::atomic<bool>> running = std::make_shared<std::atomic<bool>>(true);
    std::shared_ptr<nHttpServer::HttpServer> httpServer;
    EXPECT_NO_THROW(
        httpServer = std::make_shared<nHttpServer::HttpServer>(
            dummyLogger,
            running,
            sessions,
            8080
        );
        sessions->stopAllSessions();
        httpServer->stop();
        sessions->stopAllSessions();
    );
}