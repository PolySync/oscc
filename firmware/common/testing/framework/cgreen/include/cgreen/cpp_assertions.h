#ifndef CGREEN_CPP_ASSERTIONS_H
#define CGREEN_CPP_ASSERTIONS_H

#include "internal/stringify_token.h"

#define assert_throws(exceptionType, expr)                              \
    try {                                                               \
        expr;                                                           \
        fail_test("Expected [" STRINGIFY_TOKEN(expr) "] "               \
                "to throw [" STRINGIFY_TOKEN(exceptionType) "]");       \
    } catch (const exceptionType& ex) {                                 \
        pass_test();                                                    \
    } catch (const exceptionType* ex) {	                                \
        pass_test();                                                    \
    }

#endif
