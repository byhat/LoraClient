#pragma once

#include "ILogger.hpp"
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>

namespace infrastructure {

class SpdlogLogger : public ILogger {
public:
    SpdlogLogger();
    ~SpdlogLogger() override = default;
    void log(LogLevel level, const std::string &msg) override;
private:
    std::shared_ptr<spdlog::logger> logger_;
};

} // namespace infrastructure
