#pragma once

#include <string>
#include <cstddef>

#include <spdlog/common.h>

namespace template_insight {

/// Logger configuration loaded from a JSON config file.
struct LoggerConfig {
    spdlog::level::level_enum level = spdlog::level::info;
    std::string filePath = "template_insight.log";
    std::size_t maxFileSize = 5 * 1024 * 1024; // 5 MB by default
    std::size_t maxFiles = 3;
};

/// Application-wide configuration (extendable later).
struct AppConfig {
    LoggerConfig logger;
};

/// Helper to convert a textual log level ("info", "debug", ...) to spdlog enum.
/// Unknown strings fall back to spdlog::level::info.
spdlog::level::level_enum parseLogLevel(const std::string& levelStr);

/// Load application configuration from a JSON file.
///
/// Expected JSON structure:
/// {
///   "logger": {
///     "level": "info",
///     "file": "logs/template_insight.log",
///     "max_size": 1048576,
///     "max_files": 5
///   }
/// }
///
/// Missing fields are replaced by defaults.
/// Unknown fields are ignored.
///
/// @throws std::runtime_error if the file cannot be read or JSON is invalid.
AppConfig loadConfigFromJsonFile(const std::string& path);

/// Initialize spdlog logging according to the given logger configuration.
///
/// This will create (or replace) a global rotating logger named "template_insight".
void initLogging(const LoggerConfig& cfg);

} // namespace template_insight
