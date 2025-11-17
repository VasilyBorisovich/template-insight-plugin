#include "api.hpp"

#include <sstream>

#include <spdlog/spdlog.h>

#include "issues.hpp"

namespace template_insight {

namespace {

std::string jsonEscape(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 8);
    for (char c : s) {
        switch (c) {
            case '\\': out += "\\\\"; break;
            case '"':  out += "\\\""; break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:   out += c;      break;
        }
    }
    return out;
}

/// A tiny helper used in the stub implementation to demonstrate
/// how you could detect a specific kind of template-related error.
///
/// Now it takes IssueRegistry so we can populate the issue from metadata.
TemplateInsightResult analyzeSimpleNoMember(const std::string& logText, const IssueRegistry& registry) {
    TemplateInsightResult result;

    const std::string needle = "no member";
    auto pos = logText.find(needle);
    if (pos != std::string::npos) {
        SPDLOG_DEBUG("Detected substring '{}' at position {} in diagnostics.", needle, pos);

        TemplateIssue issue;
        issue.code = IssueCodes::NO_MEMBER;

        // Try to get metadata from the registry.
        if (auto kind = registry.find(issue.code)) {
            issue.category        = kind->category;
            issue.severity        = kind->defaultSeverity;
            issue.shortMessage    = kind->defaultShortMessage;
            issue.detailedMessage = kind->defaultDetailedMessage;
        } else {
            // Fallback hard-coded defaults if registry does not know this code.
            issue.category = "MemberAccess";
            issue.severity = Severity::Error;
            issue.shortMessage =
                "Detected 'no member' error in diagnostics.";
            issue.detailedMessage =
                "The compiler reported that a type does not have a required member.\n"
                "This is often caused by using a type that does not meet template "
                "requirements (e.g., passing an int where a container is expected).";
        }

        issue.location = std::nullopt;
        result.issues.push_back(std::move(issue));
    } else {
        SPDLOG_DEBUG("No 'no member' error pattern found in diagnostics.");
    }

    return result;
}

std::string severityToString(Severity s) {
    switch (s) {
        case Severity::Info:    return "info";
        case Severity::Warning: return "warning";
        case Severity::Error:   return "error";
    }
    return "unknown";
}

/// Utility: check if an issue code is enabled in the analysis config.
/// If enabledIssueCodes is empty, all codes are allowed.
bool isIssueCodeEnabled(const std::string& code, const AnalysisConfig& cfg) {
    if (cfg.enabledIssueCodes.empty()) {
        return true;
    }
    return std::find(cfg.enabledIssueCodes.begin(),
        cfg.enabledIssueCodes.end(),
        code) != cfg.enabledIssueCodes.end();
}

} // namespace

TemplateInsightResult analyzeDiagnostics(
    const std::string& logText,
    const AnalysisOptions& options,
    const AppConfig& config
) {
    SPDLOG_INFO("Starting diagnostics analysis. Compiler: {}, input size: {} bytes",
                options.compiler, logText.size());
    SPDLOG_DEBUG("Logger config: file='{}', max_size={}, max_files={}",
                 config.logger.filePath,
                 config.logger.maxFileSize,
                 config.logger.maxFiles);
    // Initialize issue registry.
    IssueRegistry registry;
    if (!config.analysis.issueKindsFile.empty()) {
        try {
            registry.loadFromJsonFile(config.analysis.issueKindsFile);
            SPDLOG_INFO("Loaded issue kinds from '{}'", config.analysis.issueKindsFile);
        } catch (const std::exception& ex) {
            SPDLOG_WARN("Failed to load issue kinds file '{}': {}. Falling back to built-in defaults.",
                        config.analysis.issueKindsFile, ex.what());
        }
    } else {
        SPDLOG_INFO("No issue_kinds_file specified. Using built-in issue defaults.");
    }

    // Here we eventually will run multiple checks; for now only the simple stub.
    TemplateInsightResult rawResult = analyzeSimpleNoMember(logText, registry);
    TemplateInsightResult filteredResult;

    // Apply basic filtering based on enabled issue codes and maxIssues.
    for (const auto& issue : rawResult.issues) {
        if (!isIssueCodeEnabled(issue.code, config.analysis)) {
            SPDLOG_DEBUG("Skipping issue with code '{}' due to analysis.enabledIssueCodes filter.", issue.code);
            continue;
        }

        filteredResult.issues.push_back(issue);
        if (filteredResult.issues.size() >= config.analysis.maxIssues) {
            SPDLOG_INFO("Reached max_issues limit ({}). Remaining issues will be ignored.",
                        config.analysis.maxIssues);
            break;
        }
    }

    SPDLOG_INFO("Diagnostics analysis complete. Issues found: {}", filteredResult.issues.size());
    for (const auto& issue : filteredResult.issues) {
        SPDLOG_DEBUG("Issue: code='{}', category='{}', severity='{}'",
                     issue.code,
                     issue.category,
                     severityToString(issue.severity));
    }

    return filteredResult;
}

std::string serializeToJson(const TemplateInsightResult& result) {
    std::ostringstream oss;
    oss << "{ \"issues\": [";

    for (std::size_t i = 0; i < result.issues.size(); ++i) {
        const auto& issue = result.issues[i];
        if (i > 0) {
            oss << ", ";
        }
        oss << "{";
        oss << "\"code\":\"" << jsonEscape(issue.code) << "\",";
        oss << "\"category\":\"" << jsonEscape(issue.category) << "\",";
        oss << "\"severity\":\"" << jsonEscape(severityToString(issue.severity)) << "\",";
        oss << "\"shortMessage\":\"" << jsonEscape(issue.shortMessage) << "\",";
        oss << "\"detailedMessage\":\"" << jsonEscape(issue.detailedMessage) << "\"";

        if (issue.location.has_value()) {
            const auto& loc = *issue.location;
            oss << ",\"location\":{"
                << "\"file\":\"" << jsonEscape(loc.file) << "\","
                << "\"line\":" << loc.line << ","
                << "\"column\":" << loc.column
                << "}";
        }

        oss << "}";
    }

    oss << "] }";
    return oss.str();
}

} // namespace template_insight
