// nlantau, 2022-04-14
#include <Arduino.h>

#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

void setup() {
    Serial.begin(9600);
    SerialBT.begin("ESP");
}

void loop() {
    if (Serial.available()) {
        SerialBT.write(Serial.read());
    }
    if (SerialBT.available()) {
        Serial.write(SerialBT.read());
    }
    delay(20);
}