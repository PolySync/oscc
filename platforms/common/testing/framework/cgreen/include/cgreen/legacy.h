#ifndef LEGACY_HEADER
#define LEGACY_HEADER

#include "internal/stringify_token.h"


/* Legacy style asserts:*/
#define assert_true(result) \
        (*get_test_reporter()->assert_true)(get_test_reporter(), __FILE__, __LINE__, (result), "[" STRINGIFY_TOKEN(result) "] should be true\n", NULL)
#define assert_false(result) \
        (*get_test_reporter()->assert_true)(get_test_reporter(), __FILE__, __LINE__, ! (result), "[" STRINGIFY_TOKEN(result) "] should be false\n", NULL)
#define assert_equal(tried, expected) \
        assert_equal_(__FILE__, __LINE__, STRINGIFY_TOKEN(tried), (intptr_t)(tried), (intptr_t)(expected))
#define assert_not_equal(tried, expected) \
        assert_not_equal_(__FILE__, __LINE__, STRINGIFY_TOKEN(tried), (intptr_t)(tried), (intptr_t)(expected))
#define assert_double_equal(tried, expected) \
        assert_double_equal_(__FILE__, __LINE__, STRINGIFY_TOKEN(tried), (tried), (expected))
#define assert_double_not_equal(tried, expected) \
        assert_double_not_equal_(__FILE__, __LINE__, STRINGIFY_TOKEN(tried), (tried), (expected))
#define assert_string_equal(tried, expected) \
        assert_string_equal_(__FILE__, __LINE__, STRINGIFY_TOKEN(tried), (tried), (expected))
#define assert_string_not_equal(tried, expected) \
        assert_string_not_equal_(__FILE__, __LINE__, STRINGIFY_TOKEN(tried), (tried), (expected))

#define assert_true_with_message(result, ...) \
        (*get_test_reporter()->assert_true)(get_test_reporter(), __FILE__, __LINE__, (result), __VA_ARGS__)
#define assert_false_with_message(result, ...) \
        (*get_test_reporter()->assert_true)(get_test_reporter(), __FILE__, __LINE__, ! (result), __VA_ARGS__)
#define assert_equal_with_message(tried, expected, ...) \
        (*get_test_reporter()->assert_true)(get_test_reporter(), __FILE__, __LINE__, ((tried) == (expected)), __VA_ARGS__)
#define assert_not_equal_with_message(tried, expected, ...) \
        (*get_test_reporter()->assert_true)(get_test_reporter(), __FILE__, __LINE__, ((tried) != (expected)), __VA_ARGS__)
#define assert_double_equal_with_message(tried, expected, ...) \
        (*get_test_reporter()->assert_true)(get_test_reporter(), __FILE__, __LINE__, doubles_are_equal((tried), (expected)), __VA_ARGS__)
#define assert_double_not_equal_with_message(tried, expected, ...) \
        (*get_test_reporter()->assert_true)(get_test_reporter(), __FILE__, __LINE__, doubles_are_equal((tried), (expected)), __VA_ARGS__)
#define assert_string_equal_with_message(tried, expected, ...) \
        (*get_test_reporter()->assert_true)(get_test_reporter(), __FILE__, __LINE__, strings_are_equal((tried), (expected)), __VA_ARGS__)
#define assert_string_not_equal_with_message(tried, expected, ...) \
        (*get_test_reporter()->assert_true)(get_test_reporter(), __FILE__, __LINE__, !strings_are_equal((tried), (expected)), __VA_ARGS__)

#endif
