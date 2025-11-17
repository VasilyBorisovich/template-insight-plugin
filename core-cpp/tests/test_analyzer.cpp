#include "api.hpp"
#include "config.hpp"

#include <gtest/gtest.h>

using namespace template_insight;

TEST(TemplateInsightCore, NoIssuesWhenLogIsEmpty) {
    std::string logText;
    AnalysisOptions options;
    AppConfig config; // default config

    TemplateInsightResult result = analyzeDiagnostics(logText, options, config);

    EXPECT_TRUE(result.issues.empty()) << "Empty diagnostics log should not produce any issues.";
}

TEST(TemplateInsightCore, DetectsNoMemberIssue) {
    const std::string logText =
        "main.cpp:10:5: error: no member named 'begin' in 'int'\n"
        "    x.begin();\n";

    AnalysisOptions options;
    AppConfig config;

    TemplateInsightResult result = analyzeDiagnostics(logText, options, config);

    ASSERT_EQ(result.issues.size(), 1u)
    << "Expected exactly one issue for 'no member' error.";

    const TemplateIssue& issue = result.issues.front();
    EXPECT_EQ(issue.code, IssueCodes::NO_MEMBER);
    EXPECT_EQ(issue.category, "MemberAccess");
    EXPECT_EQ(issue.severity, Severity::Error);
    EXPECT_FALSE(issue.shortMessage.empty());
    EXPECT_FALSE(issue.detailedMessage.empty());
}

TEST(TemplateInsightCore, JsonSerializationProducesNonEmptyOutput) {
    TemplateInsightResult result;
    TemplateIssue issue;
    issue.code = "NO_MEMBER";
    issue.category = "MemberAccess";
    issue.severity = Severity::Error;
    issue.shortMessage = "A short message";
    issue.detailedMessage = "A detailed message";
    result.issues.push_back(issue);

    std::string json = serializeToJson(result);

    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("\"issues\""), std::string::npos);
    EXPECT_NE(json.find("NO_MEMBER"), std::string::npos);
}
