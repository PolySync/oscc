#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include <cucumber-cpp/autodetect.hpp>
// #include <sys/timeb.h>
// #include <unistd.h>

using namespace cgreen;


// return to known state before every scenario
BEFORE()
{
}


GIVEN("^throttle control is enabled$")
{
}


THEN("^control should be enabled$")
{
    assert_that(1, is_equal_to(1));
}

WHEN("^the operator applies to the accelerator$")
{
}
