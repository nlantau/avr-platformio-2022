/***************************************************************************
 * LCD Demo
 *
 * Created: 2022-04-16
 * Updated: 2022-04-16
 * Author : nlantau
 **************************************************************************/

/***** Include guard ******************************************************/
#ifndef F_CPU
#define F_CPU 16000000UL
#endif /* F_CPU */

/***** Include section ****************************************************/
#include <avr/io.h>
#include <lcd.h>
#include <util/delay.h>

/***** Macro Definitions **************************************************/

/***** Function prototypes ************************************************/

/***** Structures *********************************************************/

/***** Global Variables ***************************************************/

/***** MAIN ***************************************************************/
int main(void) {
    lcd_init();

    for (;;) {
        // lcd_clear_row(2);

        /* TODO: Complete this mess */

        lcd_putcmd(LCD_HOME);
        lcd_puts("Hello moto     .");
        _delay_ms(1000);
        lcd_clear_row(0);
    }
    return 0;

} /* End main() */

/* End main.c */
