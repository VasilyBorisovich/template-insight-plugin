#include "issues.hpp"

#include <fstream>
#include <stdexcept>
#include <algorithm>

#include <nlohmann/json.hpp>

namespace template_insight {

using nlohmann::json;

Severity parseSeverity(const std::string& s) {
    std::string lower = s;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c){ return static_cast<char>(std::tolower(c)); });

    if (lower == "info")     return Severity::Info;
    if (lower == "warning" || lower == "warn") return Severity::Warning;
    if (lower == "error")    return Severity::Error;

    // For now, we treat unknown as Error. You can refine this later.
    return Severity::Error;
}

void IssueRegistry::loadFromJsonFile(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        throw std::runtime_error("IssueRegistry: failed to open issue kinds file: " + path);
    }

    json j;
    try {
        in >> j;
    } catch (const std::exception& ex) {
        throw std::runtime_error(std::string("IssueRegistry: failed to parse JSON: ") + ex.what());
    }

    if (!j.contains("issue_kinds") || !j["issue_kinds"].is_array()) {
        throw std::runtime_error("IssueRegistry: JSON does not contain 'issue_kinds' array");
    }

    for (const auto& item : j["issue_kinds"]) {
        if (!item.is_object()) {
            continue;
        }

        IssueKind kind;

        if (item.contains("code") && item["code"].is_string()) {
            kind.code = item["code"].get<std::string>();
        } else {
            // Skip entries without mandatory 'code'.
            continue;
        }

        if (item.contains("category") && item["category"].is_string()) {
            kind.category = item["category"].get<std::string>();
        }

        if (item.contains("default_severity") && item["default_severity"].is_string()) {
            kind.defaultSeverity = parseSeverity(item["default_severity"].get<std::string>());
        }

        if (item.contains("default_short_message") && item["default_short_message"].is_string()) {
            kind.defaultShortMessage = item["default_short_message"].get<std::string>();
        }

        if (item.contains("default_detailed_message") && item["default_detailed_message"].is_string()) {
            kind.defaultDetailedMessage = item["default_detailed_message"].get<std::string>();
        }

        addIssueKind(kind);
    }
}

void IssueRegistry::addIssueKind(const IssueKind& kind) {
    kinds_[kind.code] = kind;
}

std::optional<IssueKind> IssueRegistry::find(const std::string& code) const {
    auto it = kinds_.find(code);
    if (it == kinds_.end()) {
        return std::nullopt;
    }
    return it->second;
}

} // namespace template_insight
