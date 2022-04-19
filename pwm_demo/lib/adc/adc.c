/***************************************************************************
 * ADC Source File
 *
 * Created: 2020-09-13
 * Updated: 2022-01-13
 * Author: nlantau
 **************************************************************************/

/***************************************************************************
 *  Info:
 *  Prescaler 128
 *  Reference 5.0V
 *  adc_read() masks out all but wanted ADC-channel
 *
 **************************************************************************/

/***** Include section ****************************************************/
#include "adc.h"


/***** ADC Initialize *****************************************************/
void adc_init(void)
{
	/* 5V reference, 8-bit */
	ADMUX |= (1 << REFS0) | (1 << ADLAR);

	/* prescaler 128 */
	ADCSRA |= (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);

	/* Enable ADC */
	ADCSRA |= (1 << ADEN);

} /* End adc_init() */


/***** ADC Read **********************************************************/
unsigned char adc_read_(uint8_t adc_channel)
{
	/* Select channel with safety mask */
	ADMUX = (ADMUX & 0xF0) | (adc_channel & 0x0F);

	/* Enable ADC, Start conversion */
	ADCSRA |= (1 << ADSC);

	/* Let conversion take its time */
	while ((ADCSRA & (1 << ADSC))) ;

	return ADCH;

} /* End adc_read_() */


void adc_read(uint8_t adc_channel)
{
	/* Select channel with safety mask */
	ADMUX = (ADMUX & 0xF0) | (adc_channel & 0x0F);

	/* Start conversion */
	ADCSRA |= (1 << ADEN) | (1 << ADSC);

	/* Let conversion take its time */
	while ((ADCSRA & (1 << ADSC))) ;

} /* End adc_read() */

