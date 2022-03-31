/***************************************************************************
 * Lec 19 - Timers
 *
 *
 * Created: 2022-03-26
 * Updated: 2022-03-30
 * Author : nlantau
 **************************************************************************/

/***** Include guard ******************************************************/
#define F_CPU 16000000UL

/***** Include section ****************************************************/
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <timers.h>
#include <uart.h>

/***** Macro Definitions **************************************************/
#define UART_ENABLE 1

/***** Function prototypes ************************************************/
static void init_leds(void);

/***** Structures *********************************************************/
#if UART_ENABLE
struct uart_config_t uart_t = {
    .init_f = uart_init,
    .puts_f = uart_puts,
    .putc_f = uart_putc,
};
#endif /* UART_ENABLE*/

struct timers_config_t timer_t = {
    .init_f_0 = timer0_init,
    .init_f_1 = timer1_init,
    .init_f_2 = timer2_init,
};

/***** MAIN ***************************************************************/

int main(void) {
    uart_t.init_f(BAUD_9600);

    timer_t.init_f_0();
    timer_t.init_f_1();
    timer_t.init_f_2();
    OCR1A = TIMER1_CTC_VALUE;

    init_leds();

    for (;;) asm volatile("nop");

    return 0;

} /* End main() */

/***** Functions **********************************************************/
static void init_leds(void) {
    DDRD |= (1 << PIND3) | (1 << PIND6);
    DDRB |= (1 << PINB1);

    PORTD &= ~((1 << PIND3) | (1 << PIND6));
    PORTB &= ~(1 << PINB1);

} /* End init_leds() */

/***** ISR ****************************************************************/
ISR(TIMER0_OVF_vect, ISR_BLOCK) {
    /* TOV0 is cleared by hw when ISR executes */
    timer_t.timer0_ovf_counter++;

    if (timer_t.timer0_ovf_counter == 61) {
        /* Critical section. Save SREG and disable ISR */
        uint8_t volatile _sreg = SREG;
        cli();

        timer_t.timer0_ovf_counter = 0;
        timer_t.timer0_seconds++;
        PORTD ^= (1 << PIND3);

#if UART_ENABLE
        sprintf(timer_t.timer0_buffer, "%d", timer_t.timer0_seconds);
        uart_t.puts_f(timer_t.timer0_buffer);
        uart_t.putc_f('\n');
#endif /* UART_ENABLE */

        /* Re-enable interrupts */
        SREG = _sreg;
    }

} /* End ISR */

ISR(TIMER1_COMPA_vect, ISR_BLOCK) {
    /* Critical section. Save SREG and disable ISR */
    uint8_t volatile _sreg = SREG;
    cli();

    PORTD ^= (1 << PIND6);
    timer_t.timer1_seconds++;

#if UART_ENABLE
    sprintf(timer_t.timer1_buffer, "%d", timer_t.timer1_seconds);
    uart_t.putc_f('\t');
    uart_t.puts_f(timer_t.timer1_buffer);
    uart_t.putc_f('\n');
#endif /* UART_ENABLE */

    /* Re-enable interrupts */
    SREG = _sreg;

} /*End ISR */

ISR(TIMER2_OVF_vect, ISR_BLOCK) {
    /* TOV0 is cleared by hw when ISR executes */
    timer_t.timer2_ovf_counter++;

    if (timer_t.timer2_ovf_counter == 61) {
        /* Critical section. Save SREG and disable ISR */
        uint8_t volatile _sreg = SREG;
        cli();

        timer_t.timer2_ovf_counter = 0;
        timer_t.timer2_seconds++;

        PORTB ^= (1 << PINB1);

#if UART_ENABLE
        sprintf(timer_t.timer2_buffer, "%d", timer_t.timer2_seconds);
        uart_t.putc_f('\t');
        uart_t.putc_f('\t');
        uart_t.puts_f(timer_t.timer2_buffer);
        uart_t.putc_f('\n');
#endif /* UART_ENABLE */

        /* Re-enable interrupts */
        SREG = _sreg;
    }

} /* End ISR */

/* End main.c */