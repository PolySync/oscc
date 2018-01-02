/**
 * @file internal/oscc.h
 * @brief Internal OSCC functionality.
 */


 #ifndef _OSCC_INTERNAL_H
 #define _OSCC_INTERNAL_H

#include <net/if.h>

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

oscc_result_t vehicle_init_can(
    const char *can_channel );

oscc_result_t oscc_can_write(
    long id,
    void *msg,
    unsigned int dlc );

oscc_result_t oscc_async_enable(
    int socket );

oscc_result_t oscc_enable_brakes(
    void );

oscc_result_t oscc_enable_steering(
    void );

oscc_result_t oscc_enable_throttle(
    void );

oscc_result_t oscc_disable_brakes(
    void );

oscc_result_t oscc_disable_steering(
    void );

oscc_result_t oscc_disable_throttle(
    void );

void oscc_update_status( );

//These are all detection function for determining CAN signals
//Might be worth putting in their own location...

struct dev_name {
 struct dev_name *next, *prev;
 char name[IFNAMSIZ];
};

char * get_dev_name(char *string);

oscc_result_t add_dev_name(const char * const name, struct dev_name * const list_ptr);

oscc_result_t construct_interfaces_list(struct dev_name * const list_ptr);

#endif /* _OSCC_INTERNAL_H */
