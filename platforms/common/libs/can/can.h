#ifndef _OSCC_CAN_H_
#define _OSCC_CAN_H_

#define CAN_BAUD (CAN_500KBPS)
#define CAN_INIT_RETRY_DELAY (50)

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
void init_can(MCP_CAN can);

#endif
