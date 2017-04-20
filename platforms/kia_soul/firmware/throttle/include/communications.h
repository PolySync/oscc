#ifndef _OSCC_KIA_SOUL_THROTTLE_COMMUNICATIONS_H_
#define _OSCC_KIA_SOUL_THROTTLE_COMMUNICATIONS_H_


#include "DAC_MCP49xx.h"
#include "mcp_can.h"
#include "can.h"

#include "throttle_module.h"


void publish_ps_ctrl_throttle_report(
        kia_soul_throttle_module_s *throttle_module,
        can_frame_s *report,
        MCP_CAN &can );

void publish_timed_report(
        kia_soul_throttle_module_s *throttle_module,
        can_frame_s *report,
        MCP_CAN &can );

void process_ps_ctrl_throttle_command(
        kia_soul_throttle_module_s *throttle_module,
        const uint8_t * const rx_frame_buffer,
        DAC_MCP49xx &dac );

void handle_ready_rx_frames(
        kia_soul_throttle_module_s *throttle_module,
        can_frame_s *frame,
        can_frame_s *rx_frame_ps_ctrl_throttle_command,
        DAC_MCP49xx &dac );

void check_rx_timeouts(
        kia_soul_throttle_module_s *throttle_module,
        can_frame_s *rx_frame,
        DAC_MCP49xx &dac );

#endif
