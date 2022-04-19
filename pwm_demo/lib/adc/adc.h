/***************************************************************************
 * ADC Header File
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

/***** Include guard ******************************************************/
#ifndef ADC_H_
#define ADC_H_


/***** Include section ****************************************************/
#include <avr/io.h>


/***** Function prototypes ************************************************/

extern void adc_init(void);
extern unsigned char adc_read_(uint8_t adc_channel);
extern void adc_read(uint8_t adc_channel);

#endif /* ADC_H_ */

