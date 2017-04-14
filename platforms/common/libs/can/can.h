#ifndef _OSCC_CAN_H_
#define _OSCC_CAN_H_

#define CAN_BAUD (CAN_500KBPS)
#define CAN_INIT_RETRY_DELAY (50)
#define RX_FRAME_AVAILABLE (0)
#define RX_FRAME_UNAVAILABLE (1)


// *****************************************************
// Function:    init_can
//
// Purpose:     Initializes the CAN communication
//              Function must iterate while the CAN module initializes
//
// Returns:     void
//
// Parameters:  [in] can - An MCP_CAN object
//
// *****************************************************
void init_can(MCP_CAN &can);


// *****************************************************
// Function:    check_for_rx_frame
//
// Purpose:     Checks for CAN frame and stores it if there is one.
//
// Returns:     int - status code indicating whether a frame was received
//
// Parameters:  [in] can - An MCP_CAN object
//              [in] frame - A can_frame struct in which to store the frame.
//
// *****************************************************
int check_for_rx_frame( MCP_CAN &can, can_frame_s *frame );


#endif
