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

/***** Macro Definitions **************************************************/
#define RX_ISR 1
#define BAUD 9600
#define MYUBRR F_CPU / 16 / BAUD - 1

/***** Function prototypes ************************************************/
static void init_leds(void);
static void uart_init(void);

/***** Structures *********************************************************/
struct ring_buffer {
    volatile uint8_t head;
    volatile uint8_t tail;
    volatile uint8_t ring[1];
};

#define RING_SIZE (sizeof(struct ring_buffer) - 1)

/***** Global Variables ***************************************************/

/***** MAIN ***************************************************************/

int main(void) {
    uart_init();
    init_leds();

    for (;;) asm volatile("nop");

    return 0;

} /* End main() */

/***** UART ***************************************************************/
static void uart_init(void) {
    /* Set BUAD to 9600*/
    UBRR0H = (MYUBRR >> 8) & 0xFF;
    UBRR0L = MYUBRR;

    /* Enable RX and TX */
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

#if RX_ISR
    /* Enable RX interrupt */
    UCSR0B |= (1 << RXCIE0);
    sei();
#endif /* RX_ISR */

} /* End uart_init()*/

ISR(USART_RX_vect, ISR_BLOCK) {
    /* Check for frame error */
    if (!(UCSR0A & (1 << FE0))) {
    } else {
        /* Must read UDR0 to clear interrupt flag */
        volatile uint8_t data __attribute__((unused)) = UDR0;
    }

} /* End ISR */

/***** LEDs ***************************************************************/
static void init_leds(void) {
    DDRD |= (1 << PIND3) | (1 << PIND6);
    DDRB |= (1 << PINB1);

    PORTD &= ~((1 << PIND3) | (1 << PIND6));
    PORTB &= ~(1 << PINB1);

} /* End init_leds() */

/***** ISR ****************************************************************/
/* End main.c */
