#pragma once

#include "model.hpp"

#include <string>
#include <unordered_map>
#include <optional>

namespace template_insight {

/// Describes metadata for a particular issue kind.
/// This is typically loaded from a JSON configuration file (issue_kinds.json).
struct IssueKind {
    /// Machine-readable code, e.g. "NO_MEMBER".
    std::string code;

    /// High-level category, e.g. "MemberAccess", "OverloadResolution".
    std::string category;

    /// Default severity if not overridden by analysis logic.
    Severity defaultSeverity = Severity::Error;

    /// Default short / title message.
    std::string defaultShortMessage;

    /// Default detailed explanation.
    std::string defaultDetailedMessage;
};

/// Registry that stores known issue kinds and can provide metadata
/// for a given issue code.
///
/// The registry is intentionally lightweight and loaded from a JSON file.
/// If an issue code is not found in the registry, callers may choose to
/// fall back to hard-coded defaults.
class IssueRegistry {
public:
    IssueRegistry() = default;

    /// Load issue kinds from the given JSON file.
    ///
    /// Expected structure:
    /// {
    ///   "issue_kinds": [
    ///      { "code": "...", "category": "...", "default_severity": "error", ... },
    ///      ...
    ///   ]
    /// }
    ///
    /// @throws std::runtime_error if file cannot be read or JSON is invalid.
    void loadFromJsonFile(const std::string& path);

    /// Add or replace a single issue kind in the registry.
    void addIssueKind(const IssueKind& kind);

    /// Lookup metadata for a given issue code.
    /// Returns std::nullopt if the code is unknown.
    std::optional<IssueKind> find(const std::string& code) const;

private:
    std::unordered_map<std::string, IssueKind> kinds_;
};

/// Helper to map textual severity from JSON ("info", "warning", "error", ...) to Severity enum.
/// Unknown strings fall back to Severity::Error.
Severity parseSeverity(const std::string& s);

} // namespace template_insight
