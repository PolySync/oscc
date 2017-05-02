#include "Arduino.h"
#include <stdio.h>

unsigned long millis() {
}

void delay(unsigned long ms) {
}

void pinMode(uint8_t, uint8_t) {};
void digitalWrite(uint8_t a, uint8_t b) {
//     printf("Digital Write: %d, %d\n", a, b);
}

int digitalRead(uint8_t a) {
//     printf("Digital Read: %d\n", a);
}
int analogRead(uint8_t a) {
        // printf("Analog Read: %d\n", a);
}
void analogReference(uint8_t mode) {
        // printf("Analog Reference: %d\n", mode);
}
void analogWrite(uint8_t a, int n) {
        // printf("Analog Write: %d, %d\n", a, n);
}