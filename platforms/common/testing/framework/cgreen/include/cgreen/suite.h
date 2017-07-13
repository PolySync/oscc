#ifndef SUITE_HEADER
#define SUITE_HEADER

#include "internal/suite_internal.h"

#include <cgreen/reporter.h>
#include <cgreen/unit.h>
#include <cgreen/internal/function_macro.h>
#ifndef __cplusplus
#include <stdbool.h>
#endif


#ifdef __cplusplus
namespace cgreen {
    extern "C" {
#endif

#define create_test_suite() create_named_test_suite_(__func__, __FILE__, __LINE__)
#define create_named_test_suite(name) create_named_test_suite_(name, __FILE__, __LINE__)
#define add_test(suite, test) add_test_(suite, STRINGIFY_TOKEN(test), &spec_name(default, test))
#define add_test_with_context(suite, context, test) add_test_(suite, STRINGIFY_TOKEN(test), &spec_name(context, test))
#define add_tests(suite, ...) add_tests_(suite, #__VA_ARGS__, (CgreenTest *)__VA_ARGS__)
#define add_suite(owner, suite) add_suite_(owner, STRINGIFY_TOKEN(suite), suite)

void set_setup(TestSuite *suite, void (*set_up)(void));
void set_teardown(TestSuite *suite, void (*tear_down)(void));
int count_tests(TestSuite *suite);
bool has_test(TestSuite *suite, const char *name);
bool has_setup(TestSuite *suite);
bool has_teardown(TestSuite *suite);
void destroy_test_suite(TestSuite *suite);

#ifdef __cplusplus
    }
}
#endif


#endif
