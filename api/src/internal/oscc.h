/**
 * @file internal/oscc.h
 * @brief Internal OSCC functionality.
 */


 #ifndef _OSCC_INTERNAL_H
 #define _OSCC_INTERNAL_H

#include <net/if.h>
#include <stdbool.h>

#define UNINITIALIZED_SOCKET -1

#define CONSTRAIN(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

struct can_contains_s {
    bool is_oscc;
    bool has_vehicle;
} can_contains_default = { false, false };

typedef struct can_contains_s can_contains;

struct vehicle_can_desc_s {
    bool has_steering_angle;
    bool has_brake_pressure;
    bool has_wheel_speed;
} vehicle_can_desc_default = { false, false, false };

typedef struct vehicle_can_desc_s vehicle_can_desc;

struct oscc_can_desc_s {
    bool has_torqe_report;
    bool has_steer_report;
    bool has_brake_report;
} oscc_can_desc_default = { false, false, false };

typedef struct oscc_can_desc_s oscc_can_desc;

struct device_names_s{
    char **name;
    size_t size;
} device_names_default = { NULL, 0 };

typedef struct device_names_s device_names;

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

oscc_result_t oscc_can_write(
    long id,
    void *msg,
    unsigned int dlc );

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

oscc_result_t register_can_signal();

// Enables asynchronous callback to each socket and should only be called after
// all connections are made to prevent interrupts while making new connections.
oscc_result_t oscc_async_enable(
    int socket );

// Runs a calback function through all available socketcan signals
oscc_result_t oscc_search_can(
    can_contains(*search_callback)( const char * ),
    bool search_oscc );

// Auto detects OSCC CAN and Vehicle CAN depending on OSCC CAN returned IDs
can_contains auto_init_all_can( const char *can_channel );

// Auto detects Vehicle CAN based on vehicle header CAN IDs
can_contains auto_init_vehicle_can( const char *can_channel );

// Initializes the OSCC CAN
oscc_result_t init_oscc_can( const char *can_channel );

// Initializes the vehicle can with vehicle header CAN IDs
oscc_result_t init_vehicle_can( const char *can_channel );

// Returns socket id after initiating a socketcan connection
int init_can_socket( const char *can_channel,
                     struct timeval *tv,
                     struct can_filter *filter );

// Determines if the CAN channel contains OSCC data and/or Vehicle CAN IDs
can_contains can_detection( const char *can_channel );

// Constructs a list of all can and vcan devices
oscc_result_t construct_interfaces_list(
    struct device_names_s * const list_ptr );

// Gets the device name from a line of /proc/net/dev data
char * get_device_name( char *string );

void clear_device_names( struct device_names_s * const names_ptr );

#endif /* _OSCC_INTERNAL_H */
