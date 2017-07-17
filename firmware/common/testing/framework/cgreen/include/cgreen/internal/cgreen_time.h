#ifndef CGREEN_TIME_HEADER
#define CGREEN_TIME_HEADER

#include <stdint.h>
#include <unistd.h>

#ifdef __cplusplus
namespace cgreen {
    extern "C" {
#endif

uint32_t cgreen_time_get_current_milliseconds(void);
uint32_t cgreen_time_duration_in_milliseconds(uint32_t start_time_in_milliseconds, uint32_t end_time_in_milliseconds);

#ifdef __cplusplus
    }
}
#endif

#endif
