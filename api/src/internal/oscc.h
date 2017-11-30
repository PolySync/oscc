/**
 * @file internal/oscc.h
 * @brief Internal OSCC functionality.
 */


 #ifndef _OSCC_INTERNAL_H
 #define _OSCC_INTERNAL_H


#define CONSTRAIN(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))


void (*brake_report_callback)(
    oscc_brake_report_s *report );

void (*steering_report_callback)(
    oscc_steering_report_s *report );

void (*throttle_report_callback)(
    oscc_throttle_report_s *report );

void (*fault_report_callback)(
    oscc_fault_report_s *report );

void (*obd_frame_callback)(
    struct can_frame *frame );


oscc_result_t oscc_init_can(
    const char *can_channel );

oscc_result_t oscc_can_write(
    long id,
    void *msg,
    unsigned int dlc );

oscc_result_t oscc_async_enable(
    int socket );

void oscc_update_status( );


#endif /* _OSCC_INTERNAL_H */
