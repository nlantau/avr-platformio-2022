/***************************************************************************
 *  Ultrasonice Header
 *
 *
 *  Created: 2020-09-21
 *  Updated: 2022-01-13
 *  Author: nlantau
 **************************************************************************/

/***** Include guard ******************************************************/

#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

/***** Include section ****************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <stdlib.h>
#include <stdio.h>


/***** Macro Definitions **************************************************/
#define echo_pin PIND2
#define trig_pin PIND3


/***** Global Variables ***************************************************/
extern volatile float resulter;
extern volatile int stoper;


/***** Function prototypes ************************************************/
extern void ultrasonic_init_trig_echo(void);
extern void ultrasonic_init_INT0(void);
extern void ultrasonic_init_timer2(void);
extern void ultrasonic_trig_pulse(void);


#endif /* ULTRASONIC_H_ */

/*** end of file ***/
