#include "SPI.h"
#include "mcp_can.h"
#include "can_frame.h"
#include "common.h"




#define CAN_CS 53                          // chip select pin for CAN Shield

// construct the CAN shield object
MCP_CAN CAN(CAN_CS);                                    // Set CS pin for the CAN shield


// Power driver pins
int powerDriverPins[] = {
  3,5,6,7,8
};

int val = 0;


// accumulator pump motor
const byte PIN_PUMP = 49;


// brake spoofer relay pin definitions
const byte PIN_BRAKE_SWITCH = 48;


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


static void init_serial( void )
{
    Serial.begin( SERIAL_BAUD );
}



static void init_can ( void )
{
    // wait until we have initialized
    while( CAN.begin(CAN_BAUD) != CAN_OK )
    {
        // wait a little
        delay( CAN_INIT_RETRY_DELAY );
        Serial.println( "init_can: retrying" );
    }

    // debug log
    Serial.println( "init_can: pass" );
}

// BENCH TESTS


// Test that the Arduino powers up
void test_power() {
    Serial.println( "test_power: passed" );
}


// in order to test that each PWM driving circuit functions correctly, we can
// drive LEDS, one by one, in a breathing patter (increasing and decreasing PWM duty cycle).
void test_power_drivers() {
    Serial.println( "Starting power driver test" );


    // starting at 0% duty cycle, step up to 100% duty cycle (255)
    for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) {
        // sets the value (range from 0 to 255):
        analogWrite(0, fadeValue);
        analogWrite(1, fadeValue);
        analogWrite(2, fadeValue);
        analogWrite(3, fadeValue);
        analogWrite(4, fadeValue);
        analogWrite(5, fadeValue);
        analogWrite(6, fadeValue);
        analogWrite(7, fadeValue);
        // wait for 30 milliseconds to see the dimming effect
        delay(100);
        val = analogRead(A3);    // read the input pin
        Serial.println(val);
    }

    // fade from 100% duty cycle back to 0% duty cycle
    for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 5) {
        // sets the value (range from 0 to 255):
        analogWrite(0, fadeValue);
        analogWrite(1, fadeValue);
        analogWrite(2, fadeValue);
        analogWrite(3, fadeValue);
        analogWrite(4, fadeValue);
        analogWrite(5, fadeValue);
        analogWrite(6, fadeValue);
        analogWrite(7, fadeValue);
        // wait for 30 milliseconds to see the dimming effect
        delay(100);
    }

}


// to test that the pump relay functions corretly we can blink an LED using
// the pump relay.
void test_pump_relay() {

    digitalWrite(PIN_PUMP, HIGH);
    delay(1000);
    digitalWrite(PIN_PUMP, LOW);
    delay(1000);
}


// to test that the brake light switch spoofing relay functions we can blink two
// LEDS in an alternating pattering using the the switch relay.
void test_switch_relay() {

    // flush all previous received and transmitted data
    Serial.flush();

    while(!Serial.available()) {
        digitalWrite(PIN_BRAKE_SWITCH, HIGH);
        delay(1000);
        digitalWrite(PIN_BRAKE_SWITCH, LOW);
        delay(1000);
    }
}

// send a CAN frame, to be recieved by some module on a CAN bus.
void test_CAN_send() {

    int cantxValue = 60;

    Serial.print("cantxValue: ");
    Serial.println(cantxValue);
    //Create data packet for CAN message
    unsigned char canMsg[8] = {cantxValue, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    // send data:  id = 0x123, standrad frame, data len = 8, stmp: data buf
    CAN.sendMsgBuf(0x07B, 0, 8, canMsg);
    delay(250);
}


// recieve a CAN frame sent from some module on a CAN bus.
void test_CAN_recieve() {

    // local vars
    can_frame_s rx_frame;

    if( CAN.checkReceive() == CAN_MSGAVAIL )
    {
        memset( &rx_frame, 0, sizeof(rx_frame) );

        // read frame
        CAN.readMsgBufID(
                (INT32U*) &rx_frame.id,
                (INT8U*) &rx_frame.dlc,
                (INT8U*) rx_frame.data );
        Serial.print("canRxValue:");
        Serial.println(rx_frame.id);
        delay(250);
    }

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
    // set up the pin for the `test_pump_relay()` function
    pinMode( PIN_PUMP, OUTPUT );
    init_serial();
    init_can();
}

void loop() {

    // interactively run tests
    //test_power_drivers();
    test_CAN_send();
    test_CAN_recieve();
    test_pump_relay();
}

