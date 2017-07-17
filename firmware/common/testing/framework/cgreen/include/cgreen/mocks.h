#ifndef MOCKS_HEADER
#define MOCKS_HEADER

#include <cgreen/internal/mocks_internal.h>
#include <cgreen/internal/stringify_token.h>
#include <cgreen/constraint.h>
#include <cgreen/reporter.h>

#ifdef __cplusplus
namespace cgreen {
    extern "C" {
#endif

/* Program the mocked functions to expect (or not) calls when parameters match constraints...

   expect(<function>, when(<parameter>, <constraint>), ...

   expect(<function>, when(<parameter>, <constraint>), will_return(<value>));
*/
#define expect(f, ...) expect_(get_test_reporter(), STRINGIFY_TOKEN(f), __FILE__, __LINE__, (Constraint *)__VA_ARGS__ +0, (Constraint *)0)
#define always_expect(f, ...) always_expect_(get_test_reporter(), STRINGIFY_TOKEN(f), __FILE__, __LINE__, (Constraint *)__VA_ARGS__ +0, (Constraint *)0)
#define never_expect(f, ...) never_expect_(get_test_reporter(), STRINGIFY_TOKEN(f), __FILE__, __LINE__, (Constraint *)__VA_ARGS__ +0, (Constraint *)0)


#ifdef _MSC_VER
// another workaround for fundamental variadic macro deficiencies in Visual C++ 2012
#define mock(...) PP_NARG(__VA_ARGS__)(get_test_reporter(), __func__, __FILE__, __LINE__, #__VA_ARGS__ "", __VA_ARGS__)
#else
#define mock(...) PP_NARG(__VA_ARGS__)(get_test_reporter(), __func__, __FILE__, __LINE__, #__VA_ARGS__ "", __VA_ARGS__ +0)
#endif

#define when(parameter, constraint) when_(#parameter, constraint)

/* Make Cgreen mocks strict, loose or learning */
typedef enum { strict_mocks = 0, loose_mocks = 1, learning_mocks = 2 } CgreenMockMode;
void cgreen_mocks_are(CgreenMockMode);

extern const int UNLIMITED_TIME_TO_LIVE;

#ifdef __cplusplus
    }
}
#endif

#endif
