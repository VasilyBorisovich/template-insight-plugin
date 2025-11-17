#pragma once

#include <string>
#include <vector>
#include <optional>

namespace template_insight {

/// Severity level for issues found in diagnostics.
enum class Severity {
    Info,
    Warning,
    Error
};

/// Represents a source code location.
struct SourceLocation {
    std::string file;
    int line = 0;
    int column = 0;
};

/// High-level type of template-related issue.
/// You will extend this enum as you add more sophisticated analysis.
enum class TemplateIssueType {
    NoMember,
    NoMatchingFunction,
    TypeMismatch,
    SubstitutionFailure,
    ConstraintNotSatisfied,
    Unknown
};

/// Represents a single template-related issue extracted from compiler diagnostics.
struct TemplateIssue {
    std::string id;                   ///< Internal ID, e.g. "NO_MEMBER"
    std::string shortMessage;         ///< Short summary for lists / UI titles
    std::string detailedMessage;      ///< Human-readable explanation
    Severity severity = Severity::Error;
    TemplateIssueType type = TemplateIssueType::Unknown;

    /// Where the issue is reported in user code (if known).
    std::optional<SourceLocation> location;
};

/// Result of the analysis of a diagnostics log.
struct TemplateInsightResult {
    std::vector<TemplateIssue> issues;
};

} // namespace template_insight
