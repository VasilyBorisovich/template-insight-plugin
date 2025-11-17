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

/// Represents a single template-related issue extracted from compiler diagnostics.
///
/// Instead of using a hard-coded enum for issue types, we use a string-based
/// code so that new types can be added without changing the core library.
/// Example codes: "NO_MEMBER", "NO_MATCHING_FUNCTION", "TYPE_MISMATCH", ...
struct TemplateIssue {
    /// Machine-readable issue code, e.g. "NO_MEMBER".
    std::string code;

    /// High-level category used for grouping in UI, e.g. "MemberAccess", "OverloadResolution".
    std::string category;

    /// Human-readable title / short summary.
    std::string shortMessage;

    /// Detailed explanation (may span multiple lines).
    std::string detailedMessage;

    Severity severity = Severity::Error;

    /// Where the issue is reported in user code (if known).
    std::optional<SourceLocation> location;
};

/// Result of the analysis of a diagnostics log.
struct TemplateInsightResult {
    std::vector<TemplateIssue> issues;
};

/// Some well-known issue codes used by the core.
/// These are purely constants; the system is open to additional codes.
namespace IssueCodes
{
    inline constexpr const char* NO_MEMBER            = "NO_MEMBER";
    inline constexpr const char* NO_MATCHING_FUNCTION = "NO_MATCHING_FUNCTION";
    inline constexpr const char* TYPE_MISMATCH        = "TYPE_MISMATCH";
    inline constexpr const char* SUBSTITUTION_FAILURE = "SUBSTITUTION_FAILURE";
    inline constexpr const char* CONSTRAINT_NOT_SATISFIED = "CONSTRAINT_NOT_SATISFIED";
}

} // namespace template_insight
