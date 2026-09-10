#include "SpdlogLogger.hpp"
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>
#include <spdlog/pattern_formatter.h>
#include <chrono>
#include <filesystem>

namespace infrastructure {

SpdlogLogger::SpdlogLogger() {
    // Ensure logs directory exists
    std::filesystem::create_directories("logs");
    // rotating logger: max size 1GB, max files 4, file name "logs/log.txt"
    logger_ = spdlog::rotating_logger_mt("LoraClient", "logs/log.txt", 1073741824, 4);
    logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
    logger_->set_level(spdlog::level::info);
    logger_->flush_on(spdlog::level::trace);
    spdlog::flush_every(std::chrono::seconds(1));
}

void SpdlogLogger::log(LogLevel level, const std::string &msg) {
    switch (level) {
        case LogLevel::Trace:   logger_->trace(msg); break;
        case LogLevel::Debug:   logger_->debug(msg); break;
        case LogLevel::Info:    logger_->info(msg); break;
        case LogLevel::Warn:    logger_->warn(msg); break;
        case LogLevel::Error:   logger_->error(msg); break;
        case LogLevel::Critical: logger_->critical(msg); break;
    }
}

} // namespace infrastructure
