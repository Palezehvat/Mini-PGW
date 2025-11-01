#include "logger.h"

namespace nLogger {

    std::shared_ptr<spdlog::logger> Logger::logger = nullptr;

    void Logger::init(const std::string& nameLogger, const std::string& pathDir,
                      const std::string& fileForLogs) {
        const auto logFilePath = std::filesystem::path(
            std::filesystem::path(pathDir) / fileForLogs
        );              
        try {
            if (!std::filesystem::exists(pathDir)) {
                std::filesystem::create_directories(pathDir);
            }
            std::ofstream ofs(logFilePath, std::ios::app);
            if (!ofs.is_open()) {
                throw std::runtime_error("Failed to create or clear log file: "
                                        + logFilePath.string());
            }
        } catch (const std::filesystem::filesystem_error& e) {
            throw std::runtime_error("Failed to create logging directory. Reason: "
                                    + std::string(e.what()));
        }

        auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_st>(
            logFilePath
        );

        fileSink->set_pattern("[%d.%m.%Y %T][%^%l]%v");
        
        logger = std::make_shared<spdlog::logger>(nameLogger, fileSink);

        spdlog::register_logger(logger);
        logger->set_level(spdlog::level::debug);
        logger->flush_on(spdlog::level::debug);
    }

    std::shared_ptr<spdlog::logger> Logger::getLogger() {
        if (!logger) {
            throw std::runtime_error("Attempt to call the logger before it is initialized");
        }
        return logger;
    }
}