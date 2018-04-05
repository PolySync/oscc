/**
 * @file oscc_timer.h
 * @brief Timer utilities.
 *
 */


#ifndef _OSCC_TIMER_H_
#define _OSCC_TIMER_H_


/*
 * @brief Maximum value that timer1 counter can contain.
 *
 */
#define TIMER1_SIZE ( 65535 )

/*
 * @brief Necessary bitshifts for a timer1 prescaler of 1.
 *
 */
#define TIMER1_PRESCALER_1 ( (_BV(CS10)) )

/*
 * @brief Necessary bitshifts for a timer1 prescaler of 8.
 *
 */
#define TIMER1_PRESCALER_8 ( (_BV(CS11)) )

/*
 * @brief Necessary bitshifts for a timer1 prescaler of 64.
 *
 */
#define TIMER1_PRESCALER_64 ( (_BV(CS11) | _BV(CS10)) )

/*
 * @brief Necessary bitshifts for a timer1 prescaler of 256.
 *
 */
#define TIMER1_PRESCALER_256 ( (_BV(CS12)) )

/*
 * @brief Necessary bitshifts for a timer1 prescaler of 1024.
 *
 */
#define TIMER1_PRESCALER_1024 ( (_BV(CS12) | _BV(CS10)) )

/*
 * @brief Maximum value that timer2 counter can contain.
 *
 */
#define TIMER2_SIZE ( 255 )

/*
 * @brief Necessary bitshifts for a timer2 prescaler of 1.
 *
 */
#define TIMER2_PRESCALER_1 ( (_BV(CS20)) )

/*
 * @brief Necessary bitshifts for a timer2 prescaler of 8.
 *
 */
#define TIMER2_PRESCALER_8 ( (_BV(CS21)) )

/*
 * @brief Necessary bitshifts for a timer2 prescaler of 32.
 *
 */
#define TIMER2_PRESCALER_32 ( (_BV(CS21) | _BV(CS20)) )

/*
 * @brief Necessary bitshifts for a timer2 prescaler of 64.
 *
 */
#define TIMER2_PRESCALER_64 ( (_BV(CS22)) )

/*
 * @brief Necessary bitshifts for a timer2 prescaler of 128.
 *
 */
#define TIMER2_PRESCALER_128 ( (_BV(CS22) | _BV(CS20)) )

/*
 * @brief Necessary bitshifts for a timer2 prescaler of 256.
 *
 */
#define TIMER2_PRESCALER_256 ( (_BV(CS22) | _BV(CS21)) )

/*
 * @brief Necessary bitshifts for a timer2 prescaler of 1024.
 *
 */
#define TIMER2_PRESCALER_1024 ( (_BV(CS22) | _BV(CS21) | _BV(CS20)) )


// ****************************************************************************
// Function:    timer1_init
//
// Purpose:     Initializes timer1 to interrupt at a set frequency and run
//              an ISR at the time of that interrupt.
//
// Notes:       timer1 is a 16-bit timer with a minimum frequency of 0.25Hz.
//
// Returns:     void
//
// Parameters:  [in] frequency - frequency at which to generate an interrupt [hz]
//              [in] isr - pointer to the interrupt service routine to call on
//                         interrupt
//
// ****************************************************************************
void timer1_init(
    float frequency,
    void (*isr)(void) );

#ifndef __AVR_ATmega32U4__
// ****************************************************************************
// Function:    timer2_init
//
// Purpose:     Initializes timer2 to interrupt at a set frequency and run
//              an ISR at the time of that interrupt.
//
// Notes:       timer2 is an 8-bit timer with a minimum frequency of 61Hz.
//
// Returns:     void
//
// Parameters:  [in] frequency - frequency at which to generate an interrupt [hz]
//              [in] isr - pointer to the interrupt service routine to call on
//                         interrupt
//
// ****************************************************************************
void timer2_init(
    float frequency,
    void (*isr)(void) );
#endif


#endif /* _OSCC_TIMER_H_ */
