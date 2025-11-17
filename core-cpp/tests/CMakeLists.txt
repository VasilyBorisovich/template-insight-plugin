add_executable(test_template_insight
    test_analyzer.cpp
    test_config.cpp
    test_issue_registry.cpp
)

target_link_libraries(test_template_insight
    PRIVATE
        template_insight_core
        GTest::gtest
        GTest::gtest_main
)

add_test(
    NAME template_insight_core_tests
    COMMAND test_template_insight
)
