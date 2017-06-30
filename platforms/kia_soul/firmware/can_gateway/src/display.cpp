/**
 * @file display.cpp
 *
 */

#include "globals.h"
#include "debug.h"
#include "brake_can_protocol.h"
#include "steering_can_protocol.h"
#include "throttle_can_protocol.h"

#include "display.h"


/*
 * @brief Pixel position of the display's X origin.
 *
 */
#define ORIGIN_X_POS ( 0 )

/*
 * @brief Pixel position of the display's Y origin.
 *
 */
#define ORIGIN_Y_POS ( 0 )

/*
 * @brief X pixel position of the DTC screen's gateway column.
 *
 */
#define GATEWAY_DTC_X_POS ( 0 )

/*
 * @brief X pixel position of the DTC screen's brake column.
 *
 */
#define BRAKE_DTC_X_POS ( 33 )

/*
 * @brief X pixel position of the DTC screen's steering column.
 *
 */
#define STEERING_DTC_X_POS ( 66 )

/*
 * @brief X pixel position of the DTC screen's throttle column.
 *
 */
#define THROTTLE_DTC_X_POS ( 99 )

/*
 * @brief Pixel height of a character on the display.
 *
 */
#define CHARACTER_HEIGHT ( 10 )


static const char * gateway_status_strings[] =
{
    "UNKNOWN",
    "GOOD",
    "WARNING",
    "ERROR"
};

static const char * module_status_strings[] =
{
    "UNKNOWN",
    "ENABLED",
    "DISABLED",
    "ERROR"
};


static void read_button( void );
static void display_status_screen( void );
static void display_dtc_screen( void );
static void print_gateway_status( gateway_status_t status );
static void print_module_status( module_status_t status );
static void print_gateway_dtcs( void );
static void print_brake_dtcs( void );
static void print_steering_dtcs( void );
static void print_throttle_dtcs( void );
static void print_dtc( const char *type, int num );
static void print_padded_number( const unsigned int number );


void update_display( void )
{
    g_display.setCursor( ORIGIN_X_POS, ORIGIN_Y_POS );
    g_display.setTextColor( WHITE, BLACK );

    read_button( );

    if( g_display_state.current_screen == STATUS_SCREEN )
    {
        display_status_screen( );
    }
    else if( g_display_state.current_screen == DTC_SCREEN )
    {
        display_dtc_screen( );
    }
}


static void read_button( void )
{
    int button_val = digitalRead( PIN_DISPLAY_BUTTON_STATUS );

    if( button_val == 1 )
    {
        g_display_state.current_screen =
            (screen_t)((g_display_state.current_screen + 1) % SCREEN_COUNT);
    }
}


static void display_status_screen( void )
{
    g_display.eraseBuffer();

    g_display.print( "Gateway:   " );
    print_gateway_status( g_display_state.status_screen.gateway );

    g_display.print( "Brakes:    " );
    print_module_status( g_display_state.status_screen.brakes );

    g_display.print( "Steering:  " );
    print_module_status( g_display_state.status_screen.steering );

    g_display.print( "Throttle:  " );
    print_module_status( g_display_state.status_screen.throttle );

    g_display.sendBuffer();
}


static void display_dtc_screen( void )
{
    g_display.eraseBuffer( );

    if( (g_display_state.status_screen.gateway == GATEWAY_STATUS_ERROR)
        || (g_display_state.status_screen.gateway == GATEWAY_STATUS_WARNING) )
    {
        print_gateway_dtcs( );
    }

    if( g_display_state.status_screen.brakes == MODULE_STATUS_ERROR )
    {
        print_brake_dtcs( );
    }

    if( g_display_state.status_screen.steering == MODULE_STATUS_ERROR )
    {
        print_steering_dtcs( );
    }

    if( g_display_state.status_screen.throttle == MODULE_STATUS_ERROR )
    {
        print_throttle_dtcs( );
    }

    g_display.sendBuffer( );
}

static void print_gateway_status( gateway_status_t status )
{
    const char * status_string = gateway_status_strings[status];

    if( status == GATEWAY_STATUS_ERROR )
    {
        g_display.setTextColor( BLACK, WHITE );
        g_display.print( status_string );
        g_display.setTextColor( WHITE, BLACK );
    }
    else
    {
        g_display.print( status_string );
    }

    g_display.print( "\n\n") ;
}


static void print_module_status( module_status_t status )
{
    const char * status_string = module_status_strings[status];

    if( status == MODULE_STATUS_ERROR )
    {
        g_display.setTextColor( BLACK, WHITE );
        g_display.print( status_string );
        g_display.setTextColor( WHITE, BLACK );
    }
    else
    {
        g_display.print( status_string );
    }

    g_display.print( "\n\n" );
}

static void print_gateway_dtcs( void )
{
    g_display.setCursor( GATEWAY_DTC_X_POS, ORIGIN_Y_POS );

    for( int dtc = 0; dtc < OSCC_GATEWAY_DTC_COUNT; ++dtc )
    {
        if( g_display_state.dtc_screen.gateway[dtc] == true )
        {
            print_dtc( "P1G", dtc );
        }

        g_display.setCursor(
            GATEWAY_DTC_X_POS,
            (CHARACTER_HEIGHT + (dtc * CHARACTER_HEIGHT)) );
    }
}


static void print_brake_dtcs( void )
{
    g_display.setCursor( BRAKE_DTC_X_POS, ORIGIN_Y_POS );

    for( int dtc = 0; dtc < OSCC_BRAKE_DTC_COUNT; ++dtc )
    {
        if( g_display_state.dtc_screen.brake[dtc] == true )
        {
            print_dtc( "P1B", dtc );
        }

        g_display.setCursor(
            BRAKE_DTC_X_POS,
            (CHARACTER_HEIGHT + (dtc * CHARACTER_HEIGHT)) );
    }
}


static void print_steering_dtcs( void )
{
    g_display.setCursor( STEERING_DTC_X_POS, ORIGIN_Y_POS ) ;

    for( int dtc = 0; dtc < OSCC_STEERING_DTC_COUNT; ++dtc )
    {
        if( g_display_state.dtc_screen.steering[dtc] == true )
        {
            print_dtc( "P1S", dtc );
        }

        g_display.setCursor(
            STEERING_DTC_X_POS,
            (CHARACTER_HEIGHT + (dtc * CHARACTER_HEIGHT)) );
    }
}


static void print_throttle_dtcs( void )
{
    g_display.setCursor( THROTTLE_DTC_X_POS, ORIGIN_Y_POS );

    for( int dtc = 0; dtc < OSCC_THROTTLE_DTC_COUNT; ++dtc )
    {
        if( g_display_state.dtc_screen.throttle[dtc] == true )
        {
            print_dtc( "P1T", dtc );
        }

        g_display.setCursor(
            THROTTLE_DTC_X_POS,
            (CHARACTER_HEIGHT + (dtc * CHARACTER_HEIGHT)) );
    }
}


static void print_dtc( const char *type, int num )
{
    g_display.print( type );
    print_padded_number( num );
}


static void print_padded_number( const unsigned int number )
{
    if( number < 10 )
    {
        g_display.print( "0" );
    }

    g_display.print( number );
}
