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
static void enable_good_led( void );
static void enable_warning_led( void );
static void enable_error_led( void );


void init_display( void )
{
    cli();
    digitalWrite(PIN_DISPLAY_LED_GOOD, LOW);
    digitalWrite(PIN_DISPLAY_LED_WARNING, LOW);
    digitalWrite(PIN_DISPLAY_LED_ERROR, LOW);

    pinMode(PIN_DISPLAY_LED_GOOD, OUTPUT);
    pinMode(PIN_DISPLAY_LED_WARNING, OUTPUT);
    pinMode(PIN_DISPLAY_LED_ERROR, OUTPUT);

    g_display.begin();
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
    if( (g_display_state.status_screen.brakes == MODULE_STATUS_ERROR)
        || (g_display_state.status_screen.steering == MODULE_STATUS_ERROR)
        || (g_display_state.status_screen.throttle == MODULE_STATUS_ERROR) )
    {
        enable_error_led( );
    }
    else if( (g_display_state.status_screen.brakes == MODULE_STATUS_UNKNOWN)
        || (g_display_state.status_screen.steering == MODULE_STATUS_UNKNOWN)
        || (g_display_state.status_screen.throttle == MODULE_STATUS_UNKNOWN) )
    {
        enable_warning_led( );
    }
    else
    {
        enable_good_led( );
    }
}


static void display_status_screen( void )
{
    g_display.eraseBuffer();

    g_display.setCursor( ORIGIN_X, ORIGIN_Y );
    g_display.print( "STATUS" );

    draw_header_line( );

    g_display.setCursor( ORIGIN_X, BRAKE_STATUS_Y );

    g_display.print( "BRAKES: " );
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
        if( g_display_state.dtc_screen.brake[dtc] == true )
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
    int button_val = digitalRead( PIN_DISPLAY_BUTTON );

    if( button_val == 1 )
    {
        g_display_state.current_screen =
            (screen_t)((g_display_state.current_screen + 1) % SCREEN_COUNT);
    }
}


static void enable_good_led( void )
{
    digitalWrite(PIN_DISPLAY_LED_GOOD, HIGH);
    digitalWrite(PIN_DISPLAY_LED_WARNING, LOW);
    digitalWrite(PIN_DISPLAY_LED_ERROR, LOW);
}


static void enable_warning_led( void )
{
    digitalWrite(PIN_DISPLAY_LED_GOOD, LOW);
    digitalWrite(PIN_DISPLAY_LED_WARNING, HIGH);
    digitalWrite(PIN_DISPLAY_LED_ERROR, LOW);
}


static void enable_error_led( void )
{
    digitalWrite(PIN_DISPLAY_LED_GOOD, LOW);
    digitalWrite(PIN_DISPLAY_LED_WARNING, LOW);
    digitalWrite(PIN_DISPLAY_LED_ERROR, HIGH);
}
