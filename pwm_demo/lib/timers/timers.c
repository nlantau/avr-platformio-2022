/***************************************************************************
 * TIMERS Source File
 *
 * Created: 2022-03-30
 * Updated: 2022-03-30
 * Author: nlantau
 **************************************************************************/

/***************************************************************************
 *  Info: See timers.h
 *
 **************************************************************************/

/***** Include section ****************************************************/
#include "timers.h"

/***** Functions **********************************************************/

/***** TIMER0 *************************************************************/

void timer0_init(void) {
    /* Normal Mode
     *
     * 8-bit, prescaler: f_cpu/1024 => 15625 timer ticks / second.
     * This requires 61 (9 ticks remainder) overflows of 8-bit register.
     *   -> 61 * 256 = 15616. 15625 - 15625 = 9.
     *
     * So, with a prescaler of f_cpu/1024 we have 1/15625 periods.
     *   -> 1/15625 = 64 * 10^-6 = 64 µs/period
     *
     * Each 8-bit timer "second" is therefore 64 * 9 = 576 µs = 0.5 ms
     * faster than a real-time second.
     *
     */

    /* f_cpu/1024 prescaler */
    TCCR0B = (1 << CS02) | (1 << CS00);

    /* Overflow Interrupt Enable*/
    TIMSK0 = (1 << TOIE0);

    sei();
}

/***** TIMER1 *************************************************************/

void timer1_init(void) {
    /* CTC Mode - Clear Timer on Compare Match
     *
     * 16-bit, prescaler: f_cpu/1024 => 15625 timer ticks / seconds.
     * This fits fine in 16 bits (>= 2^16 - 1 = 65535)
     *
     * OCR1A or ICR1 are used to manipulate the counter resolution.
     * Counter is cleared to zero when the counter value (TCNT1) matches
     * either the OCR1A or the ICR1.
     *
     */

    /* f_cpu/1024 prescaler */
    TCCR1B = (1 << CS12) | (1 << CS10);

    /* CTC Mode. Top: OCR1A */
    TCCR1B |= (1 << WGM12);

    /* Compare A Match Interrupt Enable */
    TIMSK1 |= (1 << OCIE1A);

    sei();
}

/***** TIMER2 *************************************************************/

void timer2_init(void) {
    /* Normal Mode
     *
     * 8-bit, prescaler: f_cpu/1024 => 15625 timer ticks / second.
     * This requires 61 (9 ticks remainder) overflows of 8-bit register.
     *   -> 61 * 256 = 15616. 15625 - 15625 = 9.
     *
     * So, with a prescaler of f_cpu/1024 we have 1/15625 periods.
     *   -> 1/15625 = 64 * 10^-6 = 64 µs/period
     *
     * Each 8-bit timer "second" is therefore 64 * 9 = 576 µs = 0.5 ms
     * faster than a real-time second.
     *
     */

    /* f_cpu/1024 prescaler */
    TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);

    /* Overflow Interrupt Enable*/
    TIMSK2 = (1 << TOIE2);

    sei();
}

/* End timers.c */
