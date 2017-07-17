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

using namespace cgreen;

extern INT8U g_mock_mcp_can_check_receive_return;
extern INT32U g_mock_mcp_can_read_msg_buf_id;
extern INT32U g_mock_mcp_can_send_msg_buf_id;

// return to known state before every scenario
BEFORE()
{
    g_mock_mcp_can_check_receive_return = -1;
    g_mock_mcp_can_read_msg_buf_id = 0;
    g_mock_mcp_can_send_msg_buf_id = 0;
}
