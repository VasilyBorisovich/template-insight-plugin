#pragma once

#include "model.hpp"
#include "config.hpp"

#include <string>

namespace template_insight {

/// Options for the analysis step (non-config, runtime options).
struct AnalysisOptions {
    /// Compiler family name, e.g. "clang", "gcc".
    /// For now this is mostly informational.
    std::string compiler = "clang";
};

/// Analyze raw compiler diagnostics (build log) and extract template-related issues.
///
/// @param logText Full text of the compiler output.
/// @param options Runtime analysis options (e.g. compiler kind).
/// @param config Application configuration (logging + future tuning).
TemplateInsightResult analyzeDiagnostics(
    const std::string& logText,
    const AnalysisOptions& options,
    const AppConfig& config
);

/// Serialize analysis result to a minimal JSON string.
std::string serializeToJson(const TemplateInsightResult& result);

} // namespace template_insight
