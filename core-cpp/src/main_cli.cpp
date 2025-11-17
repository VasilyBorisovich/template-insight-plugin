#include "api.hpp"
#include "config.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <spdlog/spdlog.h>

using namespace template_insight;

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    try {
        // For now we use a fixed config path. In the future you may want
        // to make this configurable via environment variable or IDE settings.
        const std::string configPath = "config.json";

        AppConfig appCfg;
        try {
            appCfg = loadConfigFromJsonFile(configPath);
        } catch (const std::exception& ex) {
            // If config is missing or invalid, we use defaults and log to stderr.
            std::cerr << "Warning: " << ex.what()
                      << ". Using default logger configuration." << std::endl;
            appCfg = AppConfig{};
        }

        // Initialize logging *before* analysis so all steps are logged.
        initLogging(appCfg.logger);

        SPDLOG_INFO("Template Insight CLI starting...");
        SPDLOG_INFO("Config file: {}", configPath);

        // Read diagnostics from stdin.
        std::ostringstream buffer;
        buffer << std::cin.rdbuf();
        const std::string logText = buffer.str();

        if (logText.empty()) {
            SPDLOG_WARN("No input received from stdin. Nothing to analyze.");
        }

        AnalysisOptions options;
        options.compiler = "clang"; // You can later drive this from config as well.

        TemplateInsightResult analysisResult =
            analyzeDiagnostics(logText, options, appCfg);

        std::string json = serializeToJson(analysisResult);
        std::cout << json << std::endl;

        SPDLOG_INFO("Template Insight CLI finished successfully.");
        return 0;

    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << std::endl;
        return 1;
    }
}
