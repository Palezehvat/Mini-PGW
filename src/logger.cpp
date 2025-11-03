#include "logger.h"

namespace nLogger {

    std::shared_ptr<spdlog::logger> Logger::logger = nullptr;

    void Logger::init(const std::string& nameLogger, const std::string& pathDir,
                      const std::string& fileForLogs, const bool& isNeedCleanLogFile) {
        const auto logFilePath = std::filesystem::path(
            std::filesystem::path(pathDir) / fileForLogs
        );
        try {
            if (!std::filesystem::exists(pathDir)) {
                std::cout << logFilePath << std::endl;
                std::filesystem::create_directories(pathDir);
            }
            if (isNeedCleanLogFile) {
                std::ofstream ofs(logFilePath, std::ios::trunc);
                if (!ofs.is_open()) {
                    throw std::runtime_error("Failed to open log file: "
                                            + logFilePath.string());
                }
            } else {
                std::ofstream ofs(logFilePath, std::ios::app);
                if (!ofs.is_open()) {
                    throw std::runtime_error("Failed to open log file: "
                                            + logFilePath.string());
                }
            }
        } catch (const std::filesystem::filesystem_error& e) {
            throw std::runtime_error("Failed to create logging directory. Reason: "
                                    + std::string(e.what()));
        }

        auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
            logFilePath
        );

        fileSink->set_pattern("[%d.%m.%Y %T][%l]%v");
        
        logger = std::make_shared<spdlog::logger>(nameLogger, fileSink);

        spdlog::register_logger(logger);
        logger->set_level(spdlog::level::debug);
        logger->flush_on(spdlog::level::debug);
        logger->info("Logger created");
    }

    std::shared_ptr<spdlog::logger> Logger::getLogger() {
        if (!logger) {
            throw std::runtime_error("Attempt to call the logger before it is initialized");
        }
        return logger;
    }
} // nLogger