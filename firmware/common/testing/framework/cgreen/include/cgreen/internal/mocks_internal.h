#ifndef MOCKS_INTERNAL_HEADER
#define MOCKS_INTERNAL_HEADER

#include <cgreen/internal/mock_table.h>
#include <cgreen/internal/function_macro.h>
#include <cgreen/constraint.h>
#include <cgreen/reporter.h>
#include <cgreen/vector.h>
#include <stdint.h>

#ifdef __cplusplus
namespace cgreen {
    extern "C" {
#endif

void expect_(TestReporter *test_reporter, const char *function, const char *test_file, int test_line, ...);
void always_expect_(TestReporter *test_reporter, const char *function, const char *test_file, int test_line, ...);
void never_expect_(TestReporter *test_reporter, const char *function, const char *test_file, int test_line, ...);
intptr_t mock_(TestReporter *test_reporter, const char *function, const char *mock_file, int mock_line, const char *parameters, ...);
Constraint *when_(const char *parameter, Constraint *constraint);

void clear_mocks(void);
void tally_mocks(TestReporter *reporter);

#ifdef __cplusplus
    }
}
#endif

#endif
