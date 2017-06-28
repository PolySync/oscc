/**
 * @file display.cpp
 *
 */

#include "globals.h"
#include "debug.h"

#include "display.h"


static const char * gateway_status_strings[] = {
    "UNKNOWN",
    "GOOD",
    "WARNING",
    "ERROR"
};

static const char * module_status_strings[] = {
    "UNKNOWN",
    "ENABLED",
    "DISABLED",
    "ERROR"
};


static void read_buttons( void );
static void display_status_screen( void );
static void display_error_screen( void );
static void print_gateway_status( gateway_status_t status );
static void print_module_status( module_status_t status );


void update_display( void )
{
    g_display.setCursor( 0, 0 );
    g_display.setTextColor( WHITE, BLACK );

    read_buttons( );

    if( g_display_state.current_screen == STATUS_SCREEN )
    {
        display_status_screen( );
    }
    else if( g_display_state.current_screen == ERROR_SCREEN )
    {
        display_error_screen( );
    }
}


static void read_buttons( void )
{
    int status_button = digitalRead( PIN_DISPLAY_BUTTON_STATUS );
    int error_button = digitalRead( PIN_DISPLAY_BUTTON_ERROR );

    if( g_display_state.current_screen == STATUS_SCREEN )
    {
        if( status_button == 1 )
        {
            g_display_state.status_screen.current_page =
                (status_page_t) ( (g_display_state.status_screen.current_page + 1)
                % STATUS_PAGE_COUNT );
        }
        else if( error_button == 1 )
        {
            g_display_state.current_screen = ERROR_SCREEN;
            g_display_state.error_screen.current_page = ERROR_PAGE_MAIN;
        }
    }
    else if( g_display_state.current_screen == ERROR_SCREEN )
    {
        if( status_button == 1 )
        {
            g_display_state.current_screen = STATUS_SCREEN;
            g_display_state.status_screen.current_page = STATUS_PAGE_MAIN;
        }
        else if( error_button == 1 )
        {
            g_display_state.error_screen.current_page =
                (error_page_t) ( (g_display_state.error_screen.current_page + 1)
                % ERROR_PAGE_COUNT );
        }
    }
}


static void display_status_screen( void )
{
    g_display.eraseBuffer();

    if( g_display_state.status_screen.current_page == STATUS_PAGE_MAIN)
    {
        g_display.print( "Gateway:   " );
        print_gateway_status( g_display_state.status_screen.gateway_status );

        g_display.print( "Brakes:    " );
        print_module_status( g_display_state.status_screen.brake_status.status );

        g_display.print( "Steering:  " );
        print_module_status( g_display_state.status_screen.steering_status );

        g_display.print( "Throttle:  " );
        print_module_status( g_display_state.status_screen.throttle_status );
    }

    g_display.sendBuffer();
}


static void display_error_screen( void )
{
    g_display.eraseBuffer( );

    if( g_display_state.error_screen.current_page == ERROR_PAGE_MAIN)
    {
        g_display.print( "Errors\n" );
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
