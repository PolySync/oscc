#ifndef BOXED_DOUBLE_HEADER
#define BOXED_DOUBLE_HEADER

#include <stdint.h>

#ifdef __cplusplus
namespace cgreen {
    extern "C" {
#endif

intptr_t box_double(double d);
double as_double(intptr_t boxed_double);
double unbox_double(intptr_t boxed_double);

typedef union {
    double value;
} BoxedDouble;

#ifdef __cplusplus
    }
}
#endif

#endif /* BOXED_DOUBLE_HEADER */
