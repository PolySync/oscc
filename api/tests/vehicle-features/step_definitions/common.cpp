#include <time.h>
#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include <cucumber-cpp/autodetect.hpp>

extern "C"
{
    #include "oscc.h"
}

#define MSEC_TO_NSEC(msec) (msec * 1000 * 1000)
#define OSCC_SLEEP_IN_MS (250)

using namespace cgreen;

static oscc_result_t ret = OSCC_ERROR;

static bool brake_enabled = false;
static bool steering_enabled = false;
static bool throttle_enabled = false;

static void send_empty_commands() 
{
    oscc_publish_steering_torque(0);
    oscc_publish_brake_position(0);
    oscc_publish_throttle_position(0);
}

static void oscc_sleep()
{
    struct timespec req = {0, MSEC_TO_NSEC(OSCC_SLEEP_IN_MS)}, rem;
    int ret = nanosleep(&req, &rem);
    while( ret == -1 && errno == EINTR) {
        // send_empty_commands();
        req.tv_sec = rem.tv_sec, req.tv_nsec = rem.tv_nsec;
        rem.tv_sec = 0, rem.tv_nsec = 0;
        ret = nanosleep(&req, &rem);
    }
}

static void throttle_callback(oscc_throttle_report_s *report)
{
    throttle_enabled = report->enabled;
}

static void steering_callback(oscc_steering_report_s *report)
{
    steering_enabled = report->enabled;
}

static void brake_callback(oscc_brake_report_s * report)
{
    brake_enabled = report->enabled;
}

// return to known state before every scenario
BEFORE_ALL()
{
    oscc_subscribe_to_steering_reports(steering_callback);
    oscc_subscribe_to_throttle_reports(throttle_callback);
    oscc_subscribe_to_brake_reports(brake_callback);
}

BEFORE()
{
    ret = OSCC_OK;
}

AFTER()
{
    oscc_disable();
    oscc_sleep();
    oscc_close(0);
    oscc_sleep();
    oscc_sleep();
    oscc_sleep();
    oscc_sleep();
    oscc_sleep();
}


GIVEN("^the oscc connection has been opened$")
{
    ret = oscc_open(0);
    oscc_sleep();
    assert_that(ret, is_equal_to(OSCC_OK));
}

GIVEN("^the oscc modules have been enabled$")
{
    ret = oscc_enable();
    assert_that(ret, is_equal_to(OSCC_OK));
    oscc_sleep();
    assert_that(steering_enabled, is_equal_to(true));
}

WHEN("^an enable command is recieved$")
{
    ret = OSCC_ERROR;
    ret = oscc_enable();
    oscc_sleep();
}

WHEN("^a disable command is recieved$")
{
    ret = OSCC_ERROR;
    ret = oscc_disable();
    oscc_sleep();
}

WHEN("^a steering command is recieved$")
{
    ret = OSCC_ERROR;
    ret = oscc_publish_steering_torque(1.0);
    oscc_sleep();
    oscc_sleep();
    oscc_sleep();
    oscc_sleep();
}

THEN("^no error should be reported$")
{
    assert_that(ret, is_equal_to(OSCC_OK));
}

THEN("^the modules should be enabled$")
{
    send_empty_commands();
    oscc_sleep();
    assert_that(steering_enabled, is_equal_to(true));
    assert_that(throttle_enabled, is_equal_to(true));
    assert_that(brake_enabled, is_equal_to(true));
}

THEN("^the modules should be disabled$")
{
    // assert_that(1, is_equal_to(1));
    send_empty_commands();
    oscc_sleep();
    assert_that(steering_enabled, is_equal_to(false));
    assert_that(throttle_enabled, is_equal_to(false));
    assert_that(brake_enabled, is_equal_to(false));
}

THEN("^the steering module should still be enabled$")
{
    send_empty_commands();
    oscc_sleep();
    assert_that(steering_enabled, is_equal_to(true));
}