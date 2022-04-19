/***************************************************************************
 *  Ultrasonice Source File
 *
 *
 *  Created: 2020-09-21
 *  Updated: 2022-01-13
 *  Author: nlantau
 **************************************************************************/

/***** Include section ****************************************************/
#include "ultrasonic.h"


/***** Global Variables ***************************************************/
volatile int timer_overflow = 0;


/***** Functions **********************************************************/
void ultrasonic_init_trig_echo(void)
{
	DDRD |= (1 << trig_pin);  /* Output trig */
	DDRD &= ~(1 << echo_pin); /* Input echo */

} /* End ultrasonic_init_trig_echo() */


void ultrasonic_init_INT0(void)
{
	EICRA |= (1 << ISC00); /* Any logical change */
	EIMSK |= (1 << INT0);  /* INT0 enable */

} /* End ultrasonic_init_INT0() */


void ultrasonic_init_timer2(void)
{
	TIMSK2 |= (1 << TOIE2); /* Timer overflow interrupt */
	TCCR2B |= (1 << CS21);  /* prescaler clk 8 */

} /* End ultrasonic_init_timer2() */


void ultrasonic_trig_pulse(void)
{
	PORTD &= ~(1 << trig_pin);
	_delay_ms(1);

	PORTD |= (1 << trig_pin);
	_delay_us(10);
	PORTD &= ~(1 << trig_pin);

} /* End ultrasonic_trigger() */


/***** Interrupt Service Routines *****************************************/
ISR(TIMER2_OVF_vect)
{
	/* 7,812 overflows == 1 second */
	timer_overflow++;

} /* End ISR() */


ISR(INT0_vect)
{
	/*************************************************
	 * ISR() executes on any logical change to INT0
	 *	 -> first call: from 0-5V
	 *					- echo_pin is high
	 *	 -> second call: from 5-0V
	 *					- echo_pin is low
	 *************************************************/
	if (PIND & (1 << echo_pin)) {
		TCNT2 = 0;
		timer_overflow = 0;
	}
	else if (!(PIND & (1 << echo_pin))) {
		(timer_overflow <= 1) ? (timer_overflow = 1) : (timer_overflow = timer_overflow);
		stoper = timer_overflow * 128 + TCNT2 / 2;

		/* ~ < 5 cm */
		if (timer_overflow < 2) resulter = timer_overflow;
		else resulter = stoper / 58.0;



		timer_overflow = 0;
		TCNT2 = 0;
	}

} /* End ISR() */

