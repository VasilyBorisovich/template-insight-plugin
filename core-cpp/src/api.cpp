#include "api.hpp"

#include <sstream>

#include <spdlog/spdlog.h>

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
TemplateInsightResult analyzeSimpleNoMember(const std::string& logText) {
    TemplateInsightResult result;

    const std::string needle = "no member";
    auto pos = logText.find(needle);
    if (pos != std::string::npos) {
        SPDLOG_DEBUG("Detected substring '{}' at position {} in diagnostics.", needle, pos);

        TemplateIssue issue;
        issue.id = "NO_MEMBER";
        issue.type = TemplateIssueType::NoMember;
        issue.severity = Severity::Error;
        issue.shortMessage = "Detected 'no member' error in diagnostics.";
        issue.detailedMessage =
            "The compiler reported that a type does not have a required member.\n"
            "This is often caused by using a type that does not meet template "
            "requirements (e.g., passing an int where a container is expected).";

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

std::string issueTypeToString(TemplateIssueType t) {
    switch (t) {
        case TemplateIssueType::NoMember:              return "NO_MEMBER";
        case TemplateIssueType::NoMatchingFunction:    return "NO_MATCHING_FUNCTION";
        case TemplateIssueType::TypeMismatch:          return "TYPE_MISMATCH";
        case TemplateIssueType::SubstitutionFailure:   return "SUBSTITUTION_FAILURE";
        case TemplateIssueType::ConstraintNotSatisfied:return "CONSTRAINT_NOT_SATISFIED";
        case TemplateIssueType::Unknown:               return "UNKNOWN";
    }
    return "UNKNOWN";
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

    // In the future, you can branch on options / config here,
    // e.g., enable/disable certain checks, limit number of issues, etc.
    TemplateInsightResult result = analyzeSimpleNoMember(logText);

    SPDLOG_INFO("Diagnostics analysis complete. Issues found: {}", result.issues.size());
    for (const auto& issue : result.issues) {
        SPDLOG_DEBUG("Issue: id='{}', type='{}', severity='{}'",
                     issue.id,
                     issueTypeToString(issue.type),
                     severityToString(issue.severity));
    }

    return result;
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
        oss << "\"id\":\"" << jsonEscape(issue.id) << "\",";
        oss << "\"type\":\"" << jsonEscape(issueTypeToString(issue.type)) << "\",";
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
