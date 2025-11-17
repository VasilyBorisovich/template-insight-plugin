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

/// Analysis-related configuration parameters.
struct AnalysisConfig {
    /// Maximum template instantiation depth the analyzer should consider.
    int maxTemplateDepth = 64;

    /// Whether to enable analysis optimizations (exact meaning is up to implementation).
    bool enableOptimizations = true;

    /// Soft timeout for analysis, in milliseconds.
    int timeoutMs = 5000;

    /// Optional list of enabled issue codes (e.g., "NO_MEMBER", "TYPE_MISMATCH").
    /// If empty, all built-in issue types are allowed.
    std::vector<std::string> enabledIssueCodes;

    /// Maximum number of issues to report before stopping analysis.
    std::size_t maxIssues = 1000;

    /// Optional path to a JSON file describing known issue kinds.
    /// If empty, a built-in minimal set (or hard-coded defaults) is used.
    std::string issueKindsFile;
};

/// Output-related configuration parameters.
struct OutputConfig {
    /// Output format identifier, e.g., "json" or "text".
    std::string format = "json";

    /// Whether to include additional details in the output.
    bool verbose = false;

    /// Optional path to an output file. If empty, output goes to stdout.
    std::string outputFile;
};

/// Application-wide configuration (extendable later).
struct AppConfig
{
    AnalysisConfig analysis;
    OutputConfig output;
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
