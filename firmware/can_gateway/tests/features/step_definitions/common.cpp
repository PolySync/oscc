#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include <cucumber-cpp/autodetect.hpp>
#include <sys/timeb.h>
#include <unistd.h>

#include "Arduino.h"
#include "communications.h"
#include "oscc_can.h"
#include "mcp_can.h"
#include "globals.h"
#include "vehicles.h"

using namespace cgreen;

extern uint8_t g_mock_mcp_can_check_receive_return;
extern uint32_t g_mock_mcp_can_read_msg_buf_id;
extern uint32_t g_mock_mcp_can_send_msg_buf_id;

// return to known state before every scenario
BEFORE()
{
    g_mock_mcp_can_check_receive_return = UINT8_MAX;
    g_mock_mcp_can_read_msg_buf_id = UINT32_MAX;
    g_mock_mcp_can_send_msg_buf_id = UINT32_MAX;
}
