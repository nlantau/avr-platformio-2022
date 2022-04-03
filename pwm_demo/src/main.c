/***************************************************************************
 * Lec 20 - PWM Demo
 *
 * Created: 2022-04-03
 * Updated: 2022-04-04
 * Author : nlantau
 **************************************************************************/

/***** Include guard ******************************************************/
#ifndef F_CPU
#define F_CPU 16000000UL
#endif /* F_CPU */

/***** Include section ****************************************************/
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

/***** Macro Definitions **************************************************/
#define RX_ISR 1
#define T1_ISR 1
#define BUFF_SIZE 6
#define TIMER1_CTC_VALUE 15625
#define BAUD 9600
#define MYUBRR F_CPU / 16 / BAUD - 1

/***** Function prototypes ************************************************/
inline static void init_leds(void);
inline static void uart_init(void);
static void uart_putc(uint8_t data);
static void uart_puts(const char *s);
static void timer0_init(void);
static void timer1_init(void);

/***** Structures *********************************************************/

/***** Global Variables ***************************************************/
static volatile uint8_t _index = 0;
static volatile uint8_t rx_data[BUFF_SIZE];
static volatile uint8_t converted_rx;

/***** MAIN ***************************************************************/
int main(void) {
    uart_init();
    timer0_init();
    timer1_init();
    init_leds();

    OCR1A = TIMER1_CTC_VALUE;

    for (;;) {
        asm volatile("nop");
    }
    return 0;

} /* End main() */

/***** UART ***************************************************************/
ISR(USART_RX_vect, ISR_BLOCK) {
    /* Check for frame error */
    if (!(UCSR0A & (1 << FE0))) {
        volatile uint8_t rx = (uint8_t)UDR0;
        rx_data[_index++] = rx;

        if (_index >= (BUFF_SIZE - 1) || rx_data[_index - 1] == '\n') {
            /* Critical section */
            volatile uint8_t _sreg = SREG;
            cli();

            /* This is NOT a pretty nor safe solution */
            rx_data[_index - 1] = '\0';
            converted_rx =
                (uint8_t)strtol((const char *)rx_data, (char **)NULL, 10);

            /* Set PWM */
            OCR0A = converted_rx;

            /* Send back and clear buffer */
            uart_puts((const char *)rx_data);
            uart_putc('\n');
            memset((void *)rx_data, '\0', BUFF_SIZE);
            _index = 0;
            SREG = _sreg;
        }

    } else {
        /* Must read UDR0 to clear interrupt flag */
        volatile uint8_t data __attribute__((unused)) = UDR0;
    }

} /* End ISR */

#ifdef __GNUC__
__attribute((always_inline))
#endif /* __GNUC__ */
inline static void
uart_init(void) {
    /* Enable RX and TX */
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    /* Disable transmission complete interrupt */
    UCSR0B &= ~(1 << TXCIE0);

    /* 8-bit character size */
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    /* Sets BAUD to 9600 */
    UBRR0H = MYUBRR >> 8;
    UBRR0L = MYUBRR;

#if RX_ISR
    /* Enable RX interrupt */
    UCSR0B |= (1 << RXCIE0);
    sei();
#endif

} /* End uart_init()*/

static void uart_putc(uint8_t data) {
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = data;

} /* uart_putc() */

static void uart_puts(const char *s) {
    while (*s) uart_putc(*s++);

} /* End uart_puts */

/***** TIMER0 *************************************************************/
static void timer0_init(void) {
    /* Timer0 is used to generate PWM */

    /* Fast PWM. TOP: 0xFF, Update OCRx at: BOTTOM, TOV flag set on: MAX */
    TCCR0A |= (1 << WGM00) | (1 << WGM01);

    /* Clear OC0A on compare match, set OC0A at BOTTOM */
    TCCR0A |= (1 << COM0A1);

    /* clk/1024 prescaler */
    TCCR0B |= (1 << CS00) | (1 << CS02);
}

/***** TIMER1 *************************************************************/
static void timer1_init(void) {
    /* Timer1 is used simply to keep track of time */

    /* fcpu/1024 prescaler */
    TCCR1B = (1 << CS12) | (1 << CS10);

    /* CTC Mode. Top: OCR1A */
    TCCR1B |= (1 << WGM12);

#if T1_ISR
    /* Compare A Match Interrupt Enable */
    TIMSK1 |= (1 << OCIE1A);
    sei();
#endif /* T1_ISR */

} /* timer1_init()*/

ISR(TIMER1_COMPA_vect, ISR_BLOCK) {
    /* Toggle LED to show ISR functionality */
    PORTB ^= (1 << PINB1);

} /* End ISR */

/***** LEDs ***************************************************************/
#ifdef __GNUC__
__attribute((always_inline))
#endif /* __GNUC__ */
inline static void
init_leds(void) {
    DDRD |= (1 << PIND3) | (1 << PIND6);
    DDRB |= (1 << PINB1);

    PORTD &= ~((1 << PIND3) | (1 << PIND6));
    PORTB &= ~(1 << PINB1);

} /* End init_leds() */

/* End main.c */
