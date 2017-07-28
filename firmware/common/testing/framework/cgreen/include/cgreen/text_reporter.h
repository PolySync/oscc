#ifndef TEXT_REPORTER_HEADER
#define TEXT_REPORTER_HEADER

#include <cgreen/reporter.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
namespace cgreen {
    extern "C" {
#endif

typedef struct text_reporter_options {
    bool use_colours;
    bool quiet_mode;
} TextReporterOptions;

TestReporter *create_text_reporter(void);

#ifdef __cplusplus
    }
}
#endif

#endif
