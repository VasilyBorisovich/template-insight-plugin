#include "config.hpp"

#include <fstream>
#include <stdexcept>
#include <algorithm>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <nlohmann/json.hpp>

namespace template_insight {

using nlohmann::json;

spdlog::level::level_enum parseLogLevel(const std::string& levelStr) {
    std::string lower = levelStr;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c){ return static_cast<char>(std::tolower(c)); });

    if (lower == "trace")    return spdlog::level::trace;
    if (lower == "debug")    return spdlog::level::debug;
    if (lower == "info")     return spdlog::level::info;
    if (lower == "warn" || lower == "warning") return spdlog::level::warn;
    if (lower == "error")    return spdlog::level::err;
    if (lower == "critical" || lower == "fatal") return spdlog::level::critical;
    if (lower == "off")      return spdlog::level::off;

    return spdlog::level::info;
}

static AnalysisConfig parseAnalysisConfig(const json& jAnalysis) {
    AnalysisConfig cfg;

    if (jAnalysis.contains("max_template_depth") && jAnalysis["max_template_depth"].is_number_integer()) {
        cfg.maxTemplateDepth = jAnalysis["max_template_depth"].get<int>();
    }
    if (jAnalysis.contains("enable_optimizations") && jAnalysis["enable_optimizations"].is_boolean()) {
        cfg.enableOptimizations = jAnalysis["enable_optimizations"].get<bool>();
    }
    if (jAnalysis.contains("timeout_ms") && jAnalysis["timeout_ms"].is_number_integer()) {
        cfg.timeoutMs = jAnalysis["timeout_ms"].get<int>();
    }
    if (jAnalysis.contains("enabled_issue_codes") && jAnalysis["enabled_issue_codes"].is_array()) {
        for (const auto& v : jAnalysis["enabled_issue_codes"]) {
            if (v.is_string()) {
                cfg.enabledIssueCodes.push_back(v.get<std::string>());
            }
        }
    }
    if (jAnalysis.contains("max_issues") && jAnalysis["max_issues"].is_number_unsigned()) {
        cfg.maxIssues = jAnalysis["max_issues"].get<std::size_t>();
    }
    if (jAnalysis.contains("issue_kinds_file") && jAnalysis["issue_kinds_file"].is_string()) {
        cfg.issueKindsFile = jAnalysis["issue_kinds_file"].get<std::string>();
    }

    return cfg;
}
static OutputConfig parseOutputConfig(const json& jOutput) {
    OutputConfig cfg;

    if (jOutput.contains("format") && jOutput["format"].is_string()) {
        cfg.format = jOutput["format"].get<std::string>();
    }
    if (jOutput.contains("verbose") && jOutput["verbose"].is_boolean()) {
        cfg.verbose = jOutput["verbose"].get<bool>();
    }
    if (jOutput.contains("output_file") && jOutput["output_file"].is_string()) {
        cfg.outputFile = jOutput["output_file"].get<std::string>();
    }

    return cfg;
}

static LoggerConfig parseLoggerConfig(const json& jLogger) {
    LoggerConfig cfg;

    if (jLogger.contains("level") && jLogger["level"].is_string()) {
        cfg.level = parseLogLevel(jLogger["level"].get<std::string>());
    }
    if (jLogger.contains("file") && jLogger["file"].is_string()) {
        cfg.filePath = jLogger["file"].get<std::string>();
    }
    if (jLogger.contains("max_size") && jLogger["max_size"].is_number_unsigned()) {
        cfg.maxFileSize = jLogger["max_size"].get<std::size_t>();
    }
    if (jLogger.contains("max_files") && jLogger["max_files"].is_number_unsigned()) {
        cfg.maxFiles = jLogger["max_files"].get<std::size_t>();
    }

    return cfg;
}

AppConfig loadConfigFromJsonFile(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        throw std::runtime_error("Failed to open config file: " + path);
    }

    json j;
    try {
        in >> j;
    } catch (const std::exception& ex) {
        throw std::runtime_error(std::string("Failed to parse JSON config: ") + ex.what());
    }

    AppConfig cfg;

    if (j.contains("logger") && j["logger"].is_object()) {
        cfg.logger = parseLoggerConfig(j["logger"]);
    }
    if (j.contains("analysis") && j["analysis"].is_object()) {
        cfg.analysis = parseAnalysisConfig(j["analysis"]);
    }
    if (j.contains("output") && j["output"].is_object()) {
        cfg.output = parseOutputConfig(j["output"]);
    }

    return cfg;
}

void initLogging(const LoggerConfig& cfg) {
    auto logger = spdlog::rotating_logger_mt(
        "template_insight",
        cfg.filePath,
        cfg.maxFileSize,
        cfg.maxFiles
    );

    logger->set_level(cfg.level);
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

    spdlog::set_default_logger(logger);
    spdlog::set_level(cfg.level);

    SPDLOG_INFO("Logging initialized. File: '{}', level: {}, max_size: {}, max_files: {}",
                cfg.filePath,
                spdlog::level::to_string_view(cfg.level),
                cfg.maxFileSize,
                cfg.maxFiles);
}

} // namespace template_insight
