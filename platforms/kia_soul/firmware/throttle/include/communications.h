#ifndef _OSCC_KIA_SOUL_THROTTLE_COMMUNICATIONS_H_
#define _OSCC_KIA_SOUL_THROTTLE_COMMUNICATIONS_H_


void publish_throttle_report( void );

void publish_timed_report( void );

void process_throttle_command( const uint8_t * const data );

void process_rx_frame( can_frame_s * const frame );

void check_for_command_timeout( void );


#endif
