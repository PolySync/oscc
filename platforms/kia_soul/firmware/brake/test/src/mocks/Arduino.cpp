#include "Arduino.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static unsigned long last_time = 0;

// callbacks for test functions


rust_callback get_low_signal;
rust_callback get_high_signal;
#define PIN_MASTER_CYLINDER_PRESSURE_SENSOR_1 ( 10 )
#define PIN_MASTER_CYLINDER_PRESSURE_SENSOR_2 ( 11 )

// just sending a new time 20 ms after our last time
unsigned long millis() {
        last_time += 20;
        return last_time * 1000;
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
        if (a == PIN_MASTER_CYLINDER_PRESSURE_SENSOR_1) {
                if(get_low_signal) {
                        return get_low_signal();
                }
                else 
                {
                        return rand() % 1800;
                }
        }
        else if (a == PIN_MASTER_CYLINDER_PRESSURE_SENSOR_2) {
                // return value bw 0, 3500 inclusive
                if(get_high_signal) {
                        return get_high_signal();
                }
                else
                {
                        return rand() % 3500;
                }
        }
}
void analogReference(uint8_t mode) {
        // printf("Analog Reference: %d\n", mode);
}
void analogWrite(uint8_t a, int n) {
        // printf("Analog Write: %d, %d\n", a, n);
}

void register_signal_callbacks(rust_callback low_signal_callback, rust_callback high_signal_callback) {
        get_low_signal = low_signal_callback;
        get_high_signal = high_signal_callback;
}