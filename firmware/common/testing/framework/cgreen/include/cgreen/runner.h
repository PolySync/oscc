#ifndef RUNNER_HEADER
#define RUNNER_HEADER

#include <cgreen/suite.h>
#include <cgreen/reporter.h>

#ifdef __cplusplus
namespace cgreen {
    extern "C" {
#endif


int run_test_suite(TestSuite *suite, TestReporter *reporter);
int run_single_test(TestSuite *suite, const char *test, TestReporter *reporter);
void die_in(unsigned int seconds);

#ifdef __cplusplus
    }
}
#endif


#endif
