#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/null_sink.h>
#include "logger.h"
#include "config_manager.h"

TEST(logger, isInitializeLogFileCorrect) {
    std::string path = "./tests/test_logger.log";
    if (std::filesystem::exists(path))
        std::filesystem::remove(path);
    EXPECT_NO_THROW(nLogger::Logger::init("test_logger", path, true));
    EXPECT_TRUE(std::filesystem::exists(path));
    std::shared_ptr<spdlog::logger> logger = nLogger::Logger::getLogger();
    EXPECT_NE(logger, nullptr);
    if (std::filesystem::exists(path))
        std::filesystem::remove(path);
}

TEST(config, isExceptionWillBeThrownIfPathToConfigIncorrect) {
    std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy4");
    std::shared_ptr<nConfigManager::ConfigManager> config 
            = std::make_shared<nConfigManager::ConfigManager>(dummyLogger);
    std::string path = std::string(PROJECT_SOURCE_DIR) 
                     + "/tests/very_important_test_config.json";
    EXPECT_THROW(config->load(path), std::runtime_error);
}

TEST(config, isExceptionWillBeThrownIfFieldInConfigHasWrongType) {
    std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy5");
    std::shared_ptr<nConfigManager::ConfigManager> configClient 
            = std::make_shared<nConfigManager::ConfigManager>(dummyLogger);
    std::string pathClient = std::string(PROJECT_SOURCE_DIR) 
                     + "/tests/configs/client/incorrect_config_1.json";
    EXPECT_THROW(configClient->load(pathClient), std::runtime_error);
    std::shared_ptr<nConfigManager::ConfigManager> configServer
            = std::make_shared<nConfigManager::ConfigManager>(dummyLogger);
    std::string pathServer = std::string(PROJECT_SOURCE_DIR) 
                     + "/tests/configs/server/incorrect_config_1.json";
    EXPECT_THROW(configServer->load(pathServer), std::runtime_error);
}

TEST(config, isExceptionWillBeThrownIfInConfigIncorrectTypeConfig) {
    std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy6");
    std::shared_ptr<nConfigManager::ConfigManager> configClient 
            = std::make_shared<nConfigManager::ConfigManager>(dummyLogger);
    std::string pathClient = std::string(PROJECT_SOURCE_DIR) 
                     + "/tests/configs/client/incorrect_config_2.json";
    EXPECT_THROW(configClient->load(pathClient), std::runtime_error);
    std::shared_ptr<nConfigManager::ConfigManager> configServer
            = std::make_shared<nConfigManager::ConfigManager>(dummyLogger);
    std::string pathServer = std::string(PROJECT_SOURCE_DIR) 
                     + "/tests/configs/server/incorrect_config_2.json";
    EXPECT_THROW(configServer->load(pathServer), std::runtime_error);
}

TEST(config, isExceptionWillBeThrownIfConfigNotContainsTypeConfig) {
    std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy7");
    std::shared_ptr<nConfigManager::ConfigManager> configClient 
            = std::make_shared<nConfigManager::ConfigManager>(dummyLogger);
    std::string pathClient = std::string(PROJECT_SOURCE_DIR) 
                     + "/tests/configs/client/incorrect_config_3.json";
    EXPECT_THROW(configClient->load(pathClient), std::runtime_error);
    std::shared_ptr<nConfigManager::ConfigManager> configServer
            = std::make_shared<nConfigManager::ConfigManager>(dummyLogger);
    std::string pathServer = std::string(PROJECT_SOURCE_DIR) 
                     + "/tests/configs/server/incorrect_config_3.json";
    EXPECT_THROW(configServer->load(pathServer), std::runtime_error);
}

TEST(config, isExceptionWillBeThrownIfConfigNotContainsSomeFieldConfig) {
    std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy8");
    std::shared_ptr<nConfigManager::ConfigManager> configClient 
            = std::make_shared<nConfigManager::ConfigManager>(dummyLogger);
    std::string pathClient = std::string(PROJECT_SOURCE_DIR) 
                     + "/tests/configs/client/incorrect_config_4.json";
    EXPECT_THROW(configClient->load(pathClient), std::runtime_error);
    std::shared_ptr<nConfigManager::ConfigManager> configServer
            = std::make_shared<nConfigManager::ConfigManager>(dummyLogger);
    std::string pathServer = std::string(PROJECT_SOURCE_DIR) 
                     + "/tests/configs/server/incorrect_config_4.json";
    EXPECT_THROW(configServer->load(pathServer), std::runtime_error);
}

TEST(config, isConfigReadCorrectlyWithTypeStartingWithLowercaseLetter) {
    std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy9");
    std::shared_ptr<nConfigManager::ConfigManager> configClient 
            = std::make_shared<nConfigManager::ConfigManager>(dummyLogger);
    std::string pathClient= std::string(PROJECT_SOURCE_DIR) 
                     + "/tests/configs/client/correct_config_1.json";
    EXPECT_NO_THROW(configClient->load(pathClient));
    std::shared_ptr<nConfigManager::ConfigManager> configServer
            = std::make_shared<nConfigManager::ConfigManager>(dummyLogger);
    std::string pathServer = std::string(PROJECT_SOURCE_DIR) 
                     + "/tests/configs/server/correct_config_1.json";
    EXPECT_NO_THROW(configServer->load(pathServer));
}

TEST(config, isConfigReadCorrectlyWithTypeStartingWithUppercaseLetter) {
    std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy10");
    std::shared_ptr<nConfigManager::ConfigManager> configClient 
            = std::make_shared<nConfigManager::ConfigManager>(dummyLogger);
    std::string pathClient = std::string(PROJECT_SOURCE_DIR) 
                     + "/tests/configs/client/correct_config_2.json";
    EXPECT_NO_THROW(configClient->load(pathClient));
    std::shared_ptr<nConfigManager::ConfigManager> configServer
            = std::make_shared<nConfigManager::ConfigManager>(dummyLogger);
    std::string pathServer = std::string(PROJECT_SOURCE_DIR) 
                     + "/tests/configs/server/correct_config_2.json";
    EXPECT_NO_THROW(configServer->load(pathServer));
}

TEST(config, isReceivingTheReadClientConfigCorrectly) {
    std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy11");
    std::shared_ptr<nConfigManager::ConfigManager> configClient 
            = std::make_shared<nConfigManager::ConfigManager>(dummyLogger);
    std::string path = std::string(PROJECT_SOURCE_DIR) 
                     + "/tests/configs/client/correct_config_1.json";
    EXPECT_NO_THROW(configClient->load(path));
    EXPECT_NO_THROW(configClient->get<nConfigManager::ConfigClient>());
}

TEST(config, isReceivingTheReadServerConfigCorrectly) {
    std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy12");
    std::shared_ptr<nConfigManager::ConfigManager> configServer 
            = std::make_shared<nConfigManager::ConfigManager>(dummyLogger);
    std::string path = std::string(PROJECT_SOURCE_DIR) 
                     + "/tests/configs/server/correct_config_1.json";
    EXPECT_NO_THROW(configServer->load(path));
    EXPECT_NO_THROW(configServer->get<nConfigManager::ConfigServer>());
}

TEST(config, isErrorWillBeReturnedUponReceiptConfigClientFromConfigServer) {
    std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy13");
    std::shared_ptr<nConfigManager::ConfigManager> configServer 
            = std::make_shared<nConfigManager::ConfigManager>(dummyLogger);
        std::string path = std::string(PROJECT_SOURCE_DIR) 
                     + "/tests/configs/server/correct_config_1.json";
    EXPECT_NO_THROW(configServer->load(path));
    EXPECT_THROW(configServer->get<nConfigManager::ConfigClient>(), std::bad_variant_access);
}

TEST(config, isErrorWillBeReturnedUponReceiptConfigServerFromConfigClient) {
    std::shared_ptr<spdlog::logger> dummyLogger = spdlog::null_logger_mt("dummy14");
    std::shared_ptr<nConfigManager::ConfigManager> configClient 
            = std::make_shared<nConfigManager::ConfigManager>(dummyLogger);
    std::string path = std::string(PROJECT_SOURCE_DIR) 
                     + "/tests/configs/client/correct_config_1.json";
    EXPECT_NO_THROW(configClient->load(path));
    EXPECT_THROW(configClient->get<nConfigManager::ConfigServer>(), std::bad_variant_access);
}