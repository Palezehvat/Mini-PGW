#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/null_sink.h>
#include "logger.h"
#include "config_manager.h"
#include "session.h"
#include "udp_server.h"
#include "http_server.h"
#include "mock_cdr.h"
#include "udp_client.h"


TEST(httpServer, isCheckSubscriberAndStopCorrect) {
    try {
        std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy1");
        auto cdr = std::make_unique<MockCDR>(dummyLogger);
        auto sessions = std::make_shared<nSessionManager::SessionManager>(
            dummyLogger,
            std::move(cdr),
            100,
            std::vector<std::string>({"123456789012345"}),
            10
        );
        std::shared_ptr<std::atomic<bool>> running = std::make_shared<std::atomic<bool>>(true);
        
        sessions->createSession("123456789012346");
    
        int port = 8081;
    
        std::shared_ptr<nHttpServer::HttpServer> httpServer 
            = std::make_shared<nHttpServer::HttpServer>(
            dummyLogger,
            running,
            sessions,
            port
        );
    
        httpServer->start();
    
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
        httplib::Client client("127.0.0.1", port);
    
        if (auto res = client.Get("/check_subscriber?imsi=123456789012346")) {
            EXPECT_EQ(res->status, 200);
            EXPECT_EQ(res->body, "active");
        } else {
            FAIL() << "Failed to get result for command /check_subscriber from http server";
        }
    
        if (auto res = client.Get("/check_subscriber?imsi=123456789012345")) {
            EXPECT_EQ(res->status, 200);
            EXPECT_EQ(res->body, "not active");
        } else {
            FAIL() << "Failed to get result for command /check_subscriber from http server for "
                      "non-existent user";
        }
    
        if (auto res = client.Post("/stop")) {
            EXPECT_EQ(res->status, 200);
            EXPECT_EQ(res->body, "stopping");
        } else {
            FAIL() << "Failed to stop server by command";
        }
        sessions->stopAllSessions();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
        EXPECT_FALSE(*running);

        EXPECT_NO_THROW(httpServer->stop(););
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    
    //EXPECT_NO_THROW(httpServer->stop(););
}

TEST(udpServerClient, isServerRespondsCreatedToValidIMSI) {
    std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy2");
    auto cdr = std::make_unique<MockCDR>(dummyLogger);
    auto sessions = std::make_shared<nSessionManager::SessionManager>(
        dummyLogger,
        std::move(cdr),
        5,
        std::vector<std::string>({"123456789012345"}),
        10
    );
    auto running = std::make_shared<std::atomic<bool>>(true);

    auto udpServer = std::make_shared<nUdpServer::UdpServer>(
        9092,
        "127.0.0.1",
        dummyLogger,
        sessions,
        running
    );
    udpServer->start();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::shared_ptr<nUdpClient::UdpClient> client;
    EXPECT_NO_THROW(
        client = std::make_shared<nUdpClient::UdpClient>(
            9092,
            "127.0.0.1",
            dummyLogger
        );
    );

    std::string response;

    EXPECT_NO_THROW(
        response = client->sendMessage("123456789012346");
    );
    EXPECT_EQ(response, "created");
    running->store(false);
    EXPECT_NO_THROW(
        udpServer->stop();
        sessions->stopAllSessions();
    );
}

TEST(udpServerClient, isServerRespondsRejectedToValidIMSI) {
    std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy3");
    auto cdr = std::make_unique<MockCDR>(dummyLogger);
    auto sessions = std::make_shared<nSessionManager::SessionManager>(
        dummyLogger,
        std::move(cdr),
        100,
        std::vector<std::string>({"123456789012345"}),
        10
    );
    auto running = std::make_shared<std::atomic<bool>>(true);

    auto udpServer = std::make_shared<nUdpServer::UdpServer>(
        9092,
        "127.0.0.1",
        dummyLogger,
        sessions,
        running
    );
    udpServer->start();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::shared_ptr<nUdpClient::UdpClient> client;
    EXPECT_NO_THROW(
        client = std::make_shared<nUdpClient::UdpClient>(
            9092,
            "127.0.0.1",
            dummyLogger
        );
    );

    std::string response;

    EXPECT_NO_THROW(
        response = client->sendMessage("123456789012345");
    );
    EXPECT_EQ(response, "rejected");

    EXPECT_NO_THROW(
        response = client->sendMessage("123456789012346");
    );
    EXPECT_EQ(response, "created");

    EXPECT_NO_THROW(
        response = client->sendMessage("123456789012346");
    );
    EXPECT_EQ(response, "rejected");

    running->store(false);
    EXPECT_NO_THROW(
        udpServer->stop();
        sessions->stopAllSessions();
    );
}