
#define CAN_CS 10                          // chip select pin for CAN Shield

#define CAN_CONTROL_BAUD (CAN_500KBPS)

#define SERIAL_DEBUG_BAUD (115200)

#define CAN_INIT_RETRY_DELAY (50)


// construct the CAN shield object
MCP_CAN CAN(CAN_CS);                                    // Set CS pin for the CAN shield


// Power driver pins
int powerDriverPins[] = {
  3,5,6,7,8
};


// accumulator pump motor
const byte PIN_PUMP = 9;     


// brake spoofer relay pin definitions
const byte PIN_BREAK_SWITCH = 48;


// sensor pin (analog) definitions
const byte PIN_PACC = 9;       // pressure accumulator sensor
const byte PIN_PMC1 = 10;      // pressure master cylinder sensor 1
const byte PIN_PMC2 = 11;      // pressure master cylinder sensor 2
const byte PIN_PRL = 12;       // pressure rear left sensor
const byte PIN_PFR = 13;       // pressure front right sensor
const byte PIN_PFL = 14;       // pressure front left sensor
const byte PIN_PRR = 15;       // pressure rear right sensor


// the following are guesses, these need to be debugged/researched
const double ZERO_PRESSURE = 0.48;        // The voltage the sensors read when no pressure is present
const double MIN_PACC = 2.3;              // minumum accumulator pressure to maintain
const double MAX_PACC = 2.4;              // max accumulator pressure to maintain
const double PEDAL_THRESH = 0.5;          // Pressure for pedal interference


int SLADutyMax,
    SLADutyMin,
    SLRDutyMax,
    SLRDutyMin;



// BENCH TESTS


// Test that the Arduino powers up
void test_power() {
    Serial.println('test_power: passed');
}


// in order to test that each PWM driving circuit functions correctly, we can
// drive LEDS, one by one, in a breathing patter (increasing and decreasing PWM).
void test_power_drivers() {

    for (int thisPin = 0; thisPin < (sizeof(powerDriverPins) / sizeof(int)); thisPin++) {

        // flush all previous received and transmitted data
        Serial.flush(); 

        // while no serial input
        while(!Serial.available()) {
            for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) {
                // sets the value (range from 0 to 255):
                analogWrite(ledPin, fadeValue);
                // wait for 30 milliseconds to see the dimming effect
                delay(30);
            }

            // fade out from max to min in increments of 5 points:
            for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 5) {
                // sets the value (range from 0 to 255):
                analogWrite(ledPin, fadeValue);
                // wait for 30 milliseconds to see the dimming effect
                delay(30);
            }

        }
    }
}


// to test that the pump relay functions corretly we can blink an LED using
// the pump relay.
void test_pump_relay() {

    // flush all previous received and transmitted data
    Serial.flush(); 

    while(!Serial.available()) {
        digitalWrite(PIN_PUMP, HIGH);   
        delay(1000);                   
        digitalWrite(PIN_PUMP, LOW);  
        delay(1000);
    }
}


// to test that the brake light switch spoofing relay functions we can blink two
// LEDS in an alternating pattering using the the switch relay.
void test_switch_relay() {

    // flush all previous received and transmitted data
    Serial.flush(); 

    while(!Serial.available()) {
        digitalWrite(PIN_BREAK_SWITCH, HIGH);   
        delay(1000);                       
        digitalWrite(PIN_BREAK_SWITCH, LOW);   
        delay(1000);
    }
}

void test_CAN_send() {
  // send a CAN frame, to be recieved by some module on a CAN bus.
}

void test_can_recieve() {
  // recieve a CAN frame sent from some module on a CAN bus.

}

// INSTALLATION TESTS

void test_pressure_response_matches_request() {
  // slowly build pressure at the brakes and check that the pressure at the brakes
  // matches the commanded pressure. This test confirms all solenoids including
  // master cylinder solenoids function as expected.
}

void test_brake_pedal_input() {
  // put the system into a braking state, press the pedal. The module should
  // exit the braking state and allow the user to brake
}

void test_brake_pedal_spoof() {
  // to test that the brake light switch spoofing relay functions we blink the
 // the brake lights.
}

void setup() {

}

void loop() {
  // interactively run tests

}
