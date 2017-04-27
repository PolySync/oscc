#ifndef _OSCC_KIA_SOUL_THROTTLE_COMMUNICATIONS_H_
#define _OSCC_KIA_SOUL_THROTTLE_COMMUNICATIONS_H_

// just for testing
void say_hello( void );

void publish_throttle_report( void );

void publish_timed_report( void );

void process_throttle_command( const uint8_t * const rx_frame_buffer );

void handle_ready_rx_frame( can_frame_s * const frame );

void check_rx_timeouts( void );


#endif
