#include "config.hpp"

#include <gtest/gtest.h>
#include <fstream>
#include <cstdio> // std::remove

using namespace template_insight;

TEST(ConfigParsing, MinimalConfigIsParsedCorrectly) {
    // Prepare a temporary JSON config file with the minimal example.
    const char* filename = "test_config_minimal.json";

    {
        std::ofstream out(filename);
        ASSERT_TRUE(out.is_open());

        out <<
          R"({
          "analysis": {
            "max_template_depth": 10,
            "enable_optimizations": true,
            "timeout_ms": 5000
          },
          "output": {
            "format": "json",
            "verbose": false,
            "output_file": "analysis_result.json"
          },
          "logger": {
            "level": "info",
            "file": "template_insight.log"
          }
        })";
    }

    AppConfig cfg = loadConfigFromJsonFile(filename);

    // Clean up the temporary file (best-effort).
    std::remove(filename);

    // ---- Check analysis section ----
    EXPECT_EQ(cfg.analysis.maxTemplateDepth, 10);
    EXPECT_TRUE(cfg.analysis.enableOptimizations);
    EXPECT_EQ(cfg.analysis.timeoutMs, 5000);
    // Defaults for optional fields:
    EXPECT_TRUE(cfg.analysis.enabledIssueCodes.empty());
    EXPECT_EQ(cfg.analysis.maxIssues, static_cast<std::size_t>(1000));

    // ---- Check output section ----
    EXPECT_EQ(cfg.output.format, "json");
    EXPECT_FALSE(cfg.output.verbose);
    EXPECT_EQ(cfg.output.outputFile, "analysis_result.json");

    // ---- Check logger section ----
    EXPECT_EQ(cfg.logger.level, spdlog::level::info);
    EXPECT_EQ(cfg.logger.filePath, "template_insight.log");
    // Defaults for optional logger fields:
    EXPECT_EQ(cfg.logger.maxFileSize, static_cast<std::size_t>(5 * 1024 * 1024));
    EXPECT_EQ(cfg.logger.maxFiles, static_cast<std::size_t>(3));
}
