#include "issues.hpp"

#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>

using namespace template_insight;

TEST(IssueRegistry, LoadSimpleIssueKindsJson) {
    const char* filename = "test_issue_kinds.json";

    {
        std::ofstream out(filename);
        ASSERT_TRUE(out.is_open());

        out <<
            R"({
              "issue_kinds": [
                {
                  "code": "NO_MEMBER",
                  "category": "MemberAccess",
                  "default_severity": "error",
                  "default_short_message": "Type does not have the required member.",
                  "default_detailed_message": "Detailed explanation here."
                }
              ]
            })";
    }

    IssueRegistry registry;
    ASSERT_NO_THROW({
        registry.loadFromJsonFile(filename);
    });

    std::remove(filename);

    auto kindOpt = registry.find("NO_MEMBER");
    ASSERT_TRUE(kindOpt.has_value());

    const IssueKind& kind = *kindOpt;
    EXPECT_EQ(kind.code, "NO_MEMBER");
    EXPECT_EQ(kind.category, "MemberAccess");
    EXPECT_EQ(kind.defaultSeverity, Severity::Error);
    EXPECT_EQ(kind.defaultShortMessage, "Type does not have the required member.");
    EXPECT_EQ(kind.defaultDetailedMessage, "Detailed explanation here.");

    auto missing = registry.find("UNKNOWN_CODE");
    EXPECT_FALSE(missing.has_value());
}
