#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include <cucumber-cpp/autodetect.hpp>

extern "C"
{
    #include "oscc.h"
}

using namespace cgreen;


// return to known state before every scenario
BEFORE()
{
}


GIVEN("^the oscc connection has been opened$")
{
    ret = oscc_open(0);
    oscc_sleep();
    assert_that(ret, is_equal_to(OSCC_OK));
}

WHEN("^an enable command is recieved$")
{
    ret = OSCC_ERROR;
    ret = oscc_enable();
}

THEN("^no error should be reported$")
{
    assert_that(ret, is_equal_to(OSCC_OK));
}

THEN("^the modules should be enabled$")
{
    send_empty_commands();
    oscc_sleep();
    // assert that OSCC sends proper can messages to enable.
}
