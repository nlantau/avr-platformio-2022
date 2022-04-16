/***************************************************************************
 * Bluetooth, UART, OLED - ESP32-WROOM-DA DevkitC v4
 *
 * Created: 2022-04-16
 * Updated: 2022-04-16
 * Author : nlantau
 **************************************************************************/

/***** Include section ****************************************************/
#include <Arduino.h>
#include <U8g2lib.h>

#include "BluetoothSerial.h"

/***** Macro Definitions **************************************************/
#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_ADDRESS 0x3C
#define WIDTH 128
#define HEIGHT 64

/***** OLED ***************************************************************/
U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
#define U8LOG_WIDTH 20
#define U8LOG_HEIGHT 8
uint8_t u8log_buffer[U8LOG_WIDTH * U8LOG_HEIGHT];
U8G2LOG u8g2log;

/***** Bluetooth **********************************************************/
BluetoothSerial SerialBT;

/***** Function Prototypes ************************************************/
void serial_rx_to_OLED_BT(char c);
void serialBT_to_OLED_UART(char *c);

/***** Main ***************************************************************/
void setup(void) {
    /* UART, baud rate 9600 */
    Serial.begin(9600);

    /* Bluetooth, name "ESP" */
    SerialBT.begin("ESP");

    /* OLED setup */
    u8g2.begin();
    u8g2.setFont(u8g2_font_5x7_tr);
    u8g2log.begin(u8g2, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
    u8g2log.setLineHeightOffset(0);
    u8g2log.setRedrawMode(1);

} /* End setup() */

void loop(void) {
    char c;

    if (SerialBT.connected()) {
        if (SerialBT.available()) {
            serialBT_to_OLED_UART(&c);
        }
        if (Serial.available()) {
            serial_rx_to_OLED_BT(Serial.read());
        }
    }

} /* End loop() */

/***** Functions **********************************************************/

void serialBT_to_OLED_UART(char *c) {
    *c = SerialBT.read();
    if (*c == '?') {
        u8g2log.print("\f");
    } else {
        u8g2log.print(*c);
    }
    Serial.write(*c);
}

void serial_rx_to_OLED_BT(char c) {
    u8g2log.print(c);
    SerialBT.write(c);

} /* End serial_rx_to_OLED_BT() */