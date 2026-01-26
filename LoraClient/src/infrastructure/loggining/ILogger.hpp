#pragma once

#include <memory>
#include <string>

namespace infrastructure {

enum class LogLevel {
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Critical
};

class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(LogLevel level, const std::string &msg) = 0;
};

using ILoggerPtr = std::shared_ptr<ILogger>;

} // namespace infrastructure
