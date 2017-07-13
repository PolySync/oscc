#ifndef CGREEN_STRING_COMPARISON_H
#define CGREEN_STRING_COMPARISON_H

#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <string.h>

#ifdef __cplusplus
namespace cgreen {
    extern "C" {
#endif

bool strings_are_equal(const char *tried, const char *expected);
bool string_contains(const char *actual, const char *expected);

#ifdef __cplusplus
    }
}
#endif

#endif
