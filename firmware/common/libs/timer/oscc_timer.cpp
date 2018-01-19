/**
 * @file oscc_timer.cpp
 *
 */


#include <Arduino.h>

#include "oscc_timer.h"


static void (*timer_1_isr)(void);
#ifdef OSCC
static void (*timer_2_isr)(void);
#elif DRIVEKIT
static void (*timer_3_isr)(void);
#endif


// timer1 interrupt service routine
ISR(TIMER1_COMPA_vect)
{
    timer_1_isr( );
}

#ifdef OSCC
// timer2 interrupt service routine
ISR(TIMER2_COMPA_vect)
{
    timer_2_isr( );
}
#elif DRIVEKIT
// timer3 interrupt service routine
ISR(TIMER3_COMPA_vect)
{
    timer_3_isr( );
}
#endif

void timer1_init( float frequency, void (*isr)(void) )
{
    // disable interrupts temporarily
    cli();

    // clear existing config
    TCCR1A = 0;
    TCCR1B = 0;

    // initialize counter value to 0
    TCNT1  = 0;


    unsigned long prescaler = F_CPU / ((TIMER1_SIZE+1) * frequency);

    if ( prescaler > 256 )
    {
        prescaler = 1024;

        TCCR1B |= TIMER1_PRESCALER_1024;
    }
    else if ( prescaler > 64 )
    {
        prescaler = 256;

        TCCR1B |= TIMER1_PRESCALER_256;
    }
    else if ( prescaler > 8 )
    {
        prescaler = 64;

        TCCR1B |= TIMER1_PRESCALER_64;
    }
    else if ( prescaler > 1 )
    {
        prescaler = 8;

        TCCR1B |= TIMER1_PRESCALER_8;
    }
    else
    {
        prescaler = 1;

        TCCR1B |= TIMER1_PRESCALER_1;
    }


    unsigned long compare_match_value = ((F_CPU) / (frequency * prescaler)) - 1;

    if ( compare_match_value > TIMER1_SIZE )
    {
        compare_match_value = TIMER1_SIZE;
    }
    else if ( compare_match_value <  1 )
    {
        compare_match_value = 1;
    }


    // set value to compare counter with
    OCR1A = compare_match_value;

    // turn on compare mode
    TCCR1B |= _BV(WGM12);

    // enable compare interrupt
    TIMSK1 |= _BV(OCIE1A);

    // attach interrupt service routine
    timer_1_isr = isr;

    // re-enable interrupts
    sei();
}

#ifdef OSCC

void timer2_init( float frequency, void (*isr)(void) )
{
    // disable interrupts temporarily
    cli();

    // clear existing config
    TCCR2A = 0;
    TCCR2B = 0;

    // initialize counter value to 0
    TCNT2  = 0;


    unsigned long prescaler = F_CPU / ((TIMER2_SIZE+1) * frequency);

    if ( prescaler > 256 )
    {
        prescaler = 1024;

        TCCR2B |= TIMER2_PRESCALER_1024;
    }
    else if ( prescaler > 128 )
    {
        prescaler = 256;

        TCCR2B |= TIMER2_PRESCALER_256;
    }
    else if ( prescaler > 64 )
    {
        prescaler = 128;

        TCCR2B |= TIMER2_PRESCALER_128;
    }
    else if ( prescaler > 32 )
    {
        prescaler = 64;

        TCCR2B |= TIMER2_PRESCALER_64;
    }
    else if ( prescaler > 8 )
    {
        prescaler = 32;

        TCCR2B |= TIMER2_PRESCALER_32;
    }
    else if ( prescaler > 1 )
    {
        prescaler = 8;

        TCCR2B |= TIMER2_PRESCALER_8;
    }
    else
    {
        prescaler = 1;

        TCCR2B |=  TIMER2_PRESCALER_1;
    }


    unsigned long compare_match_value = ((F_CPU) / (frequency * prescaler)) - 1;

    if ( compare_match_value > TIMER2_SIZE )
    {
        compare_match_value = TIMER2_SIZE;
    }
    else if ( compare_match_value <  1 )
    {
        compare_match_value = 1;
    }


    // set value to compare counter with
    OCR2A = compare_match_value;

    // turn on compare mode
    TCCR2B |= _BV(WGM21);

    // enable compare interrupt
    TIMSK2 |= _BV(OCIE2A);

    // attach interrupt service routine
    timer_2_isr = isr;

    // re-enable interrupts
    sei();
}
#elif DRIVEKIT
void timer3_init( float frequency, void (*isr)(void) )
{

    PRR1 = PRR1 & (~_BV(PRTIM3));

    // disable interrupts temporarily
    cli();

    // clear existing config
    TCCR3A = 0;
    TCCR3B = 0;

    // initialize counter value to 0
    TCNT3  = 0;


    unsigned long prescaler = F_CPU / ((TIMER2_SIZE+1) * frequency);

    if ( prescaler > 256 )
    {
        prescaler = 1024;

        TCCR3B |= TIMER3_PRESCALER_1024;
    }
    else if ( prescaler > 128 )
    {
        prescaler = 256;

        TCCR3B |= TIMER3_PRESCALER_256;
    }
    else if ( prescaler > 64 )
    {
        prescaler = 128;

        TCCR3B |= TIMER3_PRESCALER_128;
    }
    else if ( prescaler > 32 )
    {
        prescaler = 64;

        TCCR3B |= TIMER3_PRESCALER_64;
    }
    else if ( prescaler > 8 )
    {
        prescaler = 32;

        TCCR3B |= TIMER3_PRESCALER_32;
    }
    else if ( prescaler > 1 )
    {
        prescaler = 8;

        TCCR3B |= TIMER3_PRESCALER_8;
    }
    else
    {
        prescaler = 1;

        TCCR3B |=  TIMER3_PRESCALER_1;
    }


    unsigned long compare_match_value = ((F_CPU) / (frequency * prescaler)) - 1;

    if ( compare_match_value > TIMER2_SIZE )
    {
        compare_match_value = TIMER2_SIZE;
    }
    else if ( compare_match_value <  1 )
    {
        compare_match_value = 1;
    }


    // set value to compare counter with
    OCR3A = compare_match_value;

    // turn on compare mode
    TCCR3B |= _BV(WGM31);

    // enable compare interrupt
    TIMSK3 |= _BV(OCIE3A);

    // attach interrupt service routine
    timer_3_isr = isr;

    // re-enable interrupts
    sei();
}
#endif
