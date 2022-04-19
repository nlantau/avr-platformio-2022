/***************************************************************************
 * TIMERS Header File
 *
 * Created: 2022-03-30
 * Updated: 2022-03-30
 * Author: nlantau
 **************************************************************************/

/***************************************************************************
 * Info: For demo purpose only. Using Timer/Counter0/1/2 with ISR
 *
 **************************************************************************/

/***** Include guard ******************************************************/
#ifndef TIMERS_S_H_
#define TIMERS_S_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif /* F_CPU */

/***** Include section ****************************************************/
#include <avr/interrupt.h>
#include <avr/io.h>

/***** Macro Definitions **************************************************/
#define TIMER1_CTC_VALUE 15625

/***** Function prototypes ************************************************/
extern void timer0_init(void);
extern void timer1_init(void);
extern void timer2_init(void);

/***** Structures *********************************************************/
struct timers_config_t {
    void (*init_f_0)(void);
    void (*init_f_1)(void);
    void (*init_f_2)(void);

    volatile uint8_t timer0_ovf_counter;
    volatile uint8_t timer0_seconds;
    volatile char timer0_buffer[6];

    /* Using 8-bit counter un purpose */
    volatile uint8_t timer1_seconds;
    volatile char timer1_buffer[6];

    volatile uint8_t timer2_ovf_counter;
    volatile uint8_t timer2_seconds;
    volatile char timer2_buffer[6];
};

#endif /* TIMERS_S_H_ */
