/**
 * @file display.cpp
 *
 */

#include "globals.h"
#include "debug.h"
#include "can_protocols/brake_can_protocol.h"
#include "can_protocols/steering_can_protocol.h"
#include "can_protocols/throttle_can_protocol.h"

#include "display.h"


/*
 * @brief X pixel position of where display drawing begins.
 *
 */
#define ORIGIN_X ( 4 )

/*
 * @brief Y pixel position of where display drawing begins.
 *
 */
#define ORIGIN_Y ( 4 )

/*
 * @brief Y pixel position of where the horizontal header line is drawn.
 *
 */
#define HEADER_LINE_Y ( 14 )

/*
 * @brief X pixel position of where the horizontal header line starts being drawn.
 *
 */
#define HEADER_LINE_X_START ( 4 )

/*
 * @brief X pixel position of where the horizontal header line stops being drawn.
 *
 */
#define HEADER_LINE_X_STOP ( 125 )

/*
 * @brief Y pixel position of where the brake status line is drawn.
 *
 */
#define BRAKE_STATUS_Y ( 18 )

/*
 * @brief Y pixel position of where the steering status line is drawn.
 *
 */
#define STEERING_STATUS_Y ( 30 )

/*
 * @brief Y pixel position of where the throttle status line is drawn.
 *
 */
#define THROTTLE_STATUS_Y ( 42 )

/*
 * @brief Y pixel position of the the first row on the DTC screen.
 *
 */
#define DTC_Y_START ( 18 )

/*
 * @brief X pixel position of the DTC screen's brake column.
 *
 */
#define BRAKE_DTC_X ( 4 )

/*
 * @brief X pixel position of the DTC screen's steering column.
 *
 */
#define STEERING_DTC_X ( 46 )

/*
 * @brief X pixel position of the DTC screen's throttle column.
 *
 */
#define THROTTLE_DTC_X ( 88 )


/*
 * @brief Number of pixels between top of character in one row and top of character
 *        in the next row.
 *
 */
#define ROW_SPACING ( 12 )


static const char * module_status_strings[] =
{
    "UNKNOWN",
    "ENABLED",
    "DISABLED",
    "ERROR"
};


static void display_status_screen( void );
static void display_dtc_screen( void );
static void draw_header_line( void );
static void update_leds( );
static void print_module_status( module_status_t status );
static void print_brake_dtcs( void );
static void print_steering_dtcs( void );
static void print_throttle_dtcs( void );
static void print_dtc( const char *type, int num );
static void print_padded_number( const unsigned int number );
static void read_button( void );


void init_display( void )
{
    cli();

    g_display.begin();

    g_display.eraseBuffer();
    g_display.sendBuffer();

    sei();
}


void update_display( void )
{
    cli();

    update_leds( );

    g_display.setCursor( ORIGIN_X, ORIGIN_Y );
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

    sei();
}


static void update_leds( void )
{
    static uint32_t lastToggle = 0;
    static uint8_t toggleState = 0;

    if( (g_display_state.status_screen.brakes == MODULE_STATUS_ERROR)
        || (g_display_state.status_screen.steering == MODULE_STATUS_ERROR)
        || (g_display_state.status_screen.throttle == MODULE_STATUS_ERROR) )
    {
        g_display.enableRedLed( );
    }
    else if( (g_display_state.status_screen.brakes == MODULE_STATUS_DISABLED)
        && (g_display_state.status_screen.steering == MODULE_STATUS_DISABLED)
        && (g_display_state.status_screen.throttle == MODULE_STATUS_DISABLED) )
    {
	if(toggleState)
        {
	    if((millis() - lastToggle) > 500)
            {
                g_display.enableYellowLed( );
                lastToggle = millis();
                toggleState = 0;
            }
        }
        else
        {
	    if((millis() - lastToggle) > 1000)
            {
                g_display.disableLeds( );
                lastToggle = millis();
                toggleState = 1;
            }
        }
    }
    else if( (g_display_state.status_screen.brakes == MODULE_STATUS_ENABLED)
        && (g_display_state.status_screen.steering == MODULE_STATUS_ENABLED)
        && (g_display_state.status_screen.throttle == MODULE_STATUS_ENABLED) )
    {
        g_display.enableGreenLed( );
    }
    else
    {
        g_display.enableRedLed( );
    }

}


static void display_status_screen( void )
{
    g_display.eraseBuffer();

    g_display.setCursor( ORIGIN_X, ORIGIN_Y );
    g_display.print( "STATUS" );

    draw_header_line( );

    g_display.setCursor( ORIGIN_X, BRAKE_STATUS_Y );

    g_display.print( "BRAKES:   " );
    print_module_status( g_display_state.status_screen.brakes );

    g_display.setCursor( ORIGIN_X, STEERING_STATUS_Y );

    g_display.print( "STEERING: " );
    print_module_status( g_display_state.status_screen.steering );

    g_display.setCursor( ORIGIN_X, THROTTLE_STATUS_Y );

    g_display.print( "THROTTLE: " );
    print_module_status( g_display_state.status_screen.throttle );

    g_display.sendBuffer();
}


static void display_dtc_screen( void )
{
    g_display.eraseBuffer( );

    g_display.setCursor( ORIGIN_X, ORIGIN_Y );
    g_display.print( "DTCS" );

    draw_header_line( );

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


static void draw_header_line( void )
{
    g_display.setCursor( ORIGIN_X, HEADER_LINE_Y );

    int pixel;
    for( pixel = HEADER_LINE_X_START; pixel <= HEADER_LINE_X_STOP; ++pixel )
    {
        g_display.drawPixel(pixel, HEADER_LINE_Y, WHITE);
    }
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


static void print_brake_dtcs( void )
{
    g_display.setCursor( BRAKE_DTC_X, DTC_Y_START );

    int row_position = DTC_Y_START;

    for( int dtc = 0; dtc < OSCC_BRAKE_DTC_COUNT; ++dtc )
    {
        if( g_display_state.dtc_screen.brakes[dtc] == true )
        {
            g_display.setCursor(
                BRAKE_DTC_X,
                row_position );

            print_dtc( "P1B", dtc );

            row_position += ROW_SPACING;
        }
    }
}


static void print_steering_dtcs( void )
{
    g_display.setCursor( STEERING_DTC_X, DTC_Y_START ) ;

    int row_position = DTC_Y_START;

    for( int dtc = 0; dtc < OSCC_STEERING_DTC_COUNT; ++dtc )
    {
        if( g_display_state.dtc_screen.steering[dtc] == true )
        {
            g_display.setCursor(
                STEERING_DTC_X,
                row_position );

            print_dtc( "P1S", dtc );

            row_position += ROW_SPACING;
        }
    }
}


static void print_throttle_dtcs( void )
{
    g_display.setCursor( THROTTLE_DTC_X, DTC_Y_START );

    int row_position = DTC_Y_START;

    for( int dtc = 0; dtc < OSCC_THROTTLE_DTC_COUNT; ++dtc )
    {
        if( g_display_state.dtc_screen.throttle[dtc] == true )
        {
            g_display.setCursor(
                THROTTLE_DTC_X,
                row_position );

            print_dtc( "P1T", dtc );

            row_position += ROW_SPACING;
        }
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


static void read_button( void )
{
    static bool button_val_last = false;

    bool button_val_current = g_display.readButton();

    if( (button_val_current != button_val_last) // prevent multiple button presses
        && (button_val_current == true) )
    {
        g_display_state.current_screen =
            (screen_t)((g_display_state.current_screen + 1) % SCREEN_COUNT);
    }

    button_val_last = button_val_current;
}
