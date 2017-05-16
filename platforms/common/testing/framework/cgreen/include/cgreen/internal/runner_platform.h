#ifndef RUNNER_PLATFORM_HEADER
#define RUNNER_PLATFORM_HEADER

#include <cgreen/suite.h>
#include <cgreen/reporter.h>

#ifdef __cplusplus
namespace cgreen {
    extern "C" {
#endif

void run_specified_test_if_child(TestSuite *suite, TestReporter *reporter);
void run_test_in_its_own_process(TestSuite *suite, CgreenTest *test, TestReporter *reporter);
void die(const char *message, ...);
void run_the_test_code(TestSuite *suite, CgreenTest *spec, TestReporter *reporter);

#ifdef __cplusplus
    }
}
#endif

#endif

