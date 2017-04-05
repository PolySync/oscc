// Throttle control ECU firmware
// Firmware for control of 2014 Kia Soul throttle system
// Component
//    Arduino Mega
//    Seeed Studio CAN-BUS Shield, v1.2 (MCP2515)
//    Sainsmart 4 relay module
//    6 channel mosfet Board
// E Livingston, 2016

#include <SPI.h>
#include <FiniteStateMachine.h>
#include "mcp_can.h"
#include "can_frame.h"
#include "common.h"
#include "control_protocol_can.h"
#include "PID.h"




// *****************************************************
// static global data
// *****************************************************


#define PSYNC_DEBUG_FLAG

// show us if debugging
#ifdef PSYNC_DEBUG_FLAG
    #warning "PSYNC_DEBUG_FLAG defined"
    #define DEBUG_PRINTLN(x)  Serial.println(x)
    #define DEBUG_PRINT(x) Serial.print(x)
#else
    #define DEBUG_PRINTLN(x)
    #define DEBUG_PRINT(x)
#endif


// chip select pin for CAN Shield
#define CAN_CS 53

// Braking PID windup guard
#define BRAKE_PID_WINDUP_GUARD ( 500 )




// *****************************************************
// static global data
// *****************************************************


//
static uint32_t last_update_ms;


// construct the CAN shield object
MCP_CAN CAN(CAN_CS);                                    // Set CS pin for the CAN shield


//
static can_frame_s rx_frame_ps_ctrl_brake_command;


//
static can_frame_s tx_frame_ps_ctrl_brake_report;


//
static PID pidParams;

int16_t PACC_reading;
int16_t PFL_reading;
int16_t PFR_reading;
int16_t PMC1_reading;
int16_t PMC2_reading;



// *****************************************************
// static declarations
// *


// uses last_update_ms, corrects for overflow condition
static void get_update_time_delta_ms(
                const uint32_t * const time_in,
                        uint32_t * const delta_out )
{
    // check for overflow
    if( last_update_ms < (*time_in) )
    {
            // time remainder, prior to the overflow
            (*delta_out) = (UINT32_MAX - (*time_in));

            // add time since zero
            (*delta_out) += last_update_ms;
        }
    else
    {
            // normal delta
            (*delta_out) = (last_update_ms - (*time_in));
        }
}

// MOSFET pin (digital) definitions ( MOSFETs control the solenoids )
// pins are not perfectly sequential because the clock frequency of certain pins is different.
const byte PIN_SLAFL = 5;      // front left actuation
const byte PIN_SLAFR = 7;      // front right actuation
// Duty cycles of pins 6, 7, and 8 controlled by timer 4 (TCCR4B)
const byte PIN_SLRFL = 6;      // front left return
const byte PIN_SLRFR = 8;      // front right return
const byte PIN_SMC = 2;      // master cylinder solenoids (two of them)

const byte PIN_PUMP = 49;     // accumulator pump motor


// brake spoofer relay pin definitions
const byte PIN_BRAKE_SWITCH_1 = 48;


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

double pressure_req,
       pressure,
       pressure_last,
       pressureRate_target,
       pressureRate;

uint8_t incomingSerialByte;

int deltaT=10,
    currMicros,
    lastMicros = 0;

unsigned long previousMillis=0;

void waitEnter();
void waitUpdate();
void waitExit();

void brakeEnter();
void brakeUpdate();
void brakeExit();

bool controlEnabled = false;
int local_override = 0;

// initialize states
State Wait = State(waitEnter, waitUpdate, waitExit);        // Wait for brake instructions
State Brake = State(brakeEnter, brakeUpdate, brakeExit);    // Control braking


// initialize state machine, start in state: Wait
FSM brakeStateMachine = FSM(Wait);


int calculateSLADutyCycle(float pre) {
  int scaled = abs(pre) * 512;
  int scale =  map(scaled, 0, 1024, SLADutyMin, SLADutyMax ); // Works for 0x05
  return scale;
}


int calculateSLRDutyCycle(float pre) {
  int scaled = abs(pre) * 512;
  int scale =  map(scaled, 0, 1024, SLRDutyMin, SLRDutyMax); // works for 0x05
  return scale;
}

// TODO: implement this
float pressureToVoltage(int MPa) {
    return ( MPa + 217.1319446 ) / 505.5662053;
    // convert MPa pressure to equivalent voltage
    return MPa;
}

// TODO: implement this
int voltageToPressure( float voltage) {
    // convert voltage reading from sensors to pressure in MPa
    return ( voltage * 505.5662053 ) - 217.1319446;
}

// convert the ADC reading (which goes from 0 - 1023) to a voltage (0 - 5V):
float convertToVoltage(int input) {
    return input * (5.0 / 1023.0);
}


// accumulator structure
struct Accumulator {
    float _pressure = 0.0;    // pressure is initliazed at 0
    byte _sensorPin = 99;     // set to 99 to avoid and accidental assignments
    byte _controlPin = 99;
    Accumulator( byte sensorP, byte relayP );

    void updatePressure()
    {
    }


    // turn relay on or off
    void pumpOn()
    {
      digitalWrite(_controlPin, HIGH);
    }


    void pumpOff()
    {
      digitalWrite(_controlPin, LOW);
    }

    void readPumpPressure( )
    {
        PACC_reading = analogRead( _sensorPin );
    }

    // maintain accumulator pressure
    void maintainPressure()
    {
        readPumpPressure();

        _pressure = convertToVoltage( PACC_reading );

        if( _pressure < MIN_PACC )
        {
            pumpOn();
            Serial.println(_pressure);
        }

        if( _pressure > MAX_PACC )
        {
            pumpOff();
        }
    }
};




// accumulator constructor
Accumulator::Accumulator( byte sensorPin, byte controlPin )
{
  _sensorPin = sensorPin;
  _controlPin = controlPin;


  pinMode( _controlPin, OUTPUT ); // set pinmode to OUTPUT


  // initialize pump to off
  pumpOff();
}




// master Solenoid structure
struct SMC {
    float _pressure1 = 0.0; // Initialize pressures to 0.0 to avoid false values
    float _pressure2 = 0.0;
    byte _sensor1Pin = 99;
    byte _sensor2Pin = 99;
    byte _controlPin = 99;

    SMC( byte sensor1Pin, byte sensor2Pin, byte controlPin );

    void checkPedal()
    {
        // read pressures at sensors
        PMC1_reading = analogRead( _sensor1Pin );
        PMC2_reading = analogRead( _sensor2Pin );

        _pressure1 = convertToVoltage( PMC1_reading );
        _pressure2 = convertToVoltage( PMC2_reading );

        // if current pedal pressure is greater than limit, disable
        if (_pressure1 > PEDAL_THRESH || _pressure2 > PEDAL_THRESH )
        {
#if 0
            DEBUG_PRINT("Brake Pedal Detected: ");
            DEBUG_PRINTLN(_pressure1);
#endif
            pressure_req = .48;
            local_override = 1;
            brakeStateMachine.transitionTo(Wait);
        }
        else
        {
            local_override = 0;
        }
    }

    void solenoidsClose()
    {
        analogWrite( _controlPin, 255 );
    }

    void solenoidsOpen()
    {
        analogWrite( _controlPin, 0 );
    }
};




SMC::SMC( byte sensor1Pin, byte sensor2Pin, byte controlPin )
{
  _sensor1Pin = sensor1Pin;
  _sensor2Pin = sensor2Pin;
  _controlPin = controlPin;

  pinMode( _controlPin, OUTPUT );  // We're writing to pin, set as an output

  solenoidsOpen();
}




// wheel structure
struct Brakes {
    float _pressureLeft = 0.0;            // last known right-side pressure
    float _pressureRight = 0.0;           // last known left-side pressure
    byte _sensorPinLeft = 99;             // pin associated with left-side  pressure sensor
    byte _sensorPinRight = 99;            // pin associated with right-side pressure sensors
    byte _solenoidPinLeftA = 99;          // pin associated with MOSFET, associated with actuation solenoid
    byte _solenoidPinRightA = 99;         // pin associated with MOSFET, associated with return solenoid
    byte _solenoidPinLeftR = 99;          // pin associated with MOSFET, associated with actuation solenoid
    byte _solenoidPinRightR = 99;         // pin associated with MOSFET, associated with return solenoid
    bool _increasingPressure = false;     // used to track if pressure should be increasing
    bool _decreasingPressure = false;     // used to track if pressure should be decreasing
    unsigned long _previousMillis = 0;    // will store last time solenoid was updated


    Brakes( byte sensorPinLeft, byte sensorPinRight, byte solenoidPinLeftA, byte solenoidPinRightA, byte solenoidPinLeftR, byte solenoidPinRightR );


    void depowerSolenoids()
    {
      analogWrite(_solenoidPinLeftA, 0);
      analogWrite(_solenoidPinRightA, 0);
      analogWrite(_solenoidPinLeftR, 0);
      analogWrite(_solenoidPinRightR, 0);

    }


    // fill pressure
    void powerSLA(int scaler)
    {
        analogWrite( _solenoidPinLeftA, scaler );
        analogWrite( _solenoidPinRightA, scaler );
    }


    void depowerSLA()
    {
        analogWrite( _solenoidPinLeftA, 0 );
        analogWrite( _solenoidPinRightA, 0 );
    }


    // spill pressure
    void powerSLR(int scaler)
    {
        Serial.println(scaler);
        analogWrite( _solenoidPinLeftR, scaler );
        analogWrite( _solenoidPinRightR, scaler );
    }


    void depowerSLR()
    {
        digitalWrite( _solenoidPinLeftR, LOW );
        digitalWrite( _solenoidPinRightR, LOW );
    }


    // take a pressure reading
    void updatePressure()
    {
        PFL_reading = analogRead( _sensorPinLeft );
        PFR_reading = analogRead(_sensorPinRight);

        _pressureLeft = convertToVoltage( PFL_reading );
        _pressureRight = convertToVoltage( PFR_reading );
    }
};

// brake constructor
Brakes::Brakes( byte sensorPLeft, byte sensorPRight, byte solenoidPinLeftA, byte solenoidPinRightA, byte solenoidPinLeftR, byte solenoidPinRightR ) {
  _sensorPinLeft = sensorPLeft;
  _sensorPinRight = sensorPRight;
  _solenoidPinLeftA = solenoidPinLeftA;
  _solenoidPinRightA = solenoidPinRightA;
  _solenoidPinLeftR = solenoidPinLeftR;
  _solenoidPinRightR = solenoidPinRightR;


  // initialize solenoid pins to off
  digitalWrite( _solenoidPinLeftA, LOW );
  digitalWrite( _solenoidPinRightA, LOW );
  digitalWrite( _solenoidPinLeftR, LOW );
  digitalWrite( _solenoidPinRightR, LOW );


  // set pinmode to OUTPUT
  pinMode( _solenoidPinLeftA, OUTPUT );
  pinMode( _solenoidPinRightA, OUTPUT );
  pinMode( _solenoidPinLeftR, OUTPUT );
  pinMode( _solenoidPinRightR, OUTPUT );
}



// Instantiate objects
Accumulator accumulator( PIN_PACC, PIN_PUMP );
SMC smc(PIN_PMC1, PIN_PMC2, PIN_SMC);
Brakes brakes = Brakes( PIN_PFL, PIN_PFR, PIN_SLAFL, PIN_SLAFR, PIN_SLRFL, PIN_SLRFR);



//
static void init_serial( void )
{
    // Disable serial
    Serial.end();

    // Init if debugging
#ifdef PSYNC_DEBUG_FLAG
    Serial.begin( SERIAL_BAUD );
#endif

    // Debug log
    DEBUG_PRINTLN( "init_serial: pass" );
}


//
static void init_can( void )
{
    // Wait until we have initialized
    while( CAN.begin(CAN_BAUD) != CAN_OK )
    {
        // wait a little
        delay( CAN_INIT_RETRY_DELAY );
    }

    // Debug log
    DEBUG_PRINTLN( "init_can: pass" );
}


// A function to parse incoming serial bytes
void processSerialByte() {
    if (incomingSerialByte == 'a') {                  // increase pressure
        pressure_req += 0.2;
    }
    if (incomingSerialByte == 'd') {                  // decrease pressure
        pressure_req -= 0.2;
    }

    if (incomingSerialByte == 'p') {                  // reset
        pressure_req = .48;
        DEBUG_PRINTLN("reset pressure request");
    }
    if (incomingSerialByte == 'q') {                  // reset
        smc.solenoidsOpen();
        DEBUG_PRINTLN("opened SMCs");
    }
    if (incomingSerialByte == 'e') {                  // reset
        smc.solenoidsClose();
        DEBUG_PRINTLN("closed SMCs");
    }
    if (incomingSerialByte == 'z') {                  // reset
        brakes.depowerSLR();
        DEBUG_PRINTLN("depower SLRs");
    }
    if (incomingSerialByte == 'c') {                  // reset
        brakes.powerSLR(255);
        DEBUG_PRINTLN("power SLRs");
    }
    if (incomingSerialByte == 'n') {                  // reset
        brakes.powerSLA(255);
        DEBUG_PRINTLN("power SLAs");
    }
    if (incomingSerialByte == 'm') {                  // reset
        brakes.depowerSLA();
        DEBUG_PRINTLN("depower SLAs");
    }
    if (incomingSerialByte == 'u') {                  // reset
        Serial.println(accumulator._pressure);
    }
    if (incomingSerialByte == 't') {                  // reset
        Serial.println("pump on");
        accumulator.pumpOn();
    }
    if (incomingSerialByte == 'y') {                  // reset
        Serial.println("pump off");
        accumulator.pumpOff();
    }
}



//
static void publish_ps_ctrl_brake_report( void )
{
    // cast data
    ps_ctrl_brake_report_msg * data =
            (ps_ctrl_brake_report_msg*) tx_frame_ps_ctrl_brake_report.data;

    // set frame ID
    tx_frame_ps_ctrl_brake_report.id = (uint32_t) (PS_CTRL_MSG_ID_BRAKE_REPORT);

    // set DLC
    tx_frame_ps_ctrl_brake_report.dlc = 8; //TODO

    // Set override flag
    data->override = local_override;

    // publish to control CAN bus
    CAN.sendMsgBuf(
            tx_frame_ps_ctrl_brake_report.id,
            0, // standard ID (not extended)
            tx_frame_ps_ctrl_brake_report.dlc,
            tx_frame_ps_ctrl_brake_report.data );

    // update last publish timestamp, ms
    tx_frame_ps_ctrl_brake_report.timestamp = last_update_ms;
}


//
static void publish_timed_tx_frames( void )
{
    // local vars
    uint32_t delta = 0;

    // get time since last publish
    get_update_time_delta_ms( &tx_frame_ps_ctrl_brake_report.timestamp, &delta );

    // check publish interval
    if( delta >= PS_CTRL_BRAKE_REPORT_PUBLISH_INTERVAL )
    {
        // publish frame, update timestamp
        publish_ps_ctrl_brake_report();
    }
}

uint16_t map_uint16(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max);


static void process_ps_ctrl_brake_command( const uint8_t * const rx_frame_buffer )
{

    // cast control frame data
    const ps_ctrl_brake_command_msg * const control_data =
            (ps_ctrl_brake_command_msg*) rx_frame_buffer;


    bool enabled = control_data->enabled == 1;

    // enable control from the MKZ interface
    if( enabled == 1 && !controlEnabled )
    {
        controlEnabled = true;
        brakeStateMachine.transitionTo(Brake);
    }

    // disable control from the MKZ interface
    if( enabled == 0 && controlEnabled )
    {
        controlEnabled = false;
        brakeStateMachine.transitionTo(Wait);
    }

    unsigned int pedal_command = control_data->pedal_command;
    pressure_req = map(pedal_command, 0, 65535, 48, 230); // map to voltage range
    pressure_req = pressure_req / 100;
    DEBUG_PRINT("pressure_req: ");
    DEBUG_PRINTLN(pressure_req);


}

static void process_psvc_chassis_state1( const uint8_t * const rx_frame_buffer )
{
    const psvc_chassis_state1_data_s * const chassis_data =
        (psvc_chassis_state1_data_s*) rx_frame_buffer;

    // brake pressure as reported from the C-CAN bus
    int brake_pressure = chassis_data->brake_pressure;

    // take a reading from the brake pressure sensors
    brakes.updatePressure();

    // average the pressure of the rear and front lines
//    float pressure = ( brakes._pressureLeft + brakes._pressureRight ) / 2;
//    DEBUG_PRINT(pressure);
//    DEBUG_PRINT(",");
    DEBUG_PRINT( "CAN," );
    DEBUG_PRINT( brake_pressure );
    DEBUG_PRINT( ",PACC," );
    DEBUG_PRINT( PACC_reading );
    DEBUG_PRINT( ",PFL," );
    DEBUG_PRINT( PFL_reading );
    DEBUG_PRINT( ",PFR," );
    DEBUG_PRINT( PFR_reading );
    DEBUG_PRINT( ",PMC1," );
    DEBUG_PRINT( PMC1_reading );
    DEBUG_PRINT( ",PMC2," );
    DEBUG_PRINTLN( PMC2_reading );
}



// a function to parse CAN data into useful variables
void handle_ready_rx_frames(void) {

    // local vars
    can_frame_s rx_frame;

    if( CAN.checkReceive() == CAN_MSGAVAIL )
    {
        memset( &rx_frame, 0, sizeof(rx_frame) );

        // update timestamp
        rx_frame.timestamp = last_update_ms;

        // read frame
        CAN.readMsgBufID(
                (INT32U*) &rx_frame.id,
                (INT8U*) &rx_frame.dlc,
                (INT8U*) rx_frame.data );

        // check for a supported frame ID
        if( rx_frame.id == PS_CTRL_MSG_ID_BRAKE_COMMAND )
        {
            // process brake command
            process_ps_ctrl_brake_command( rx_frame.data );
        }

        // check for a supported frame ID
        if( rx_frame.id == KIA_STATUS1_MESSAGE_ID )
        {
            // process brake command
            process_psvc_chassis_state1( rx_frame.data );
        }
    }

}




void waitEnter()
{
    controlEnabled = false;

    // open master cylinder solenoids
    smc.solenoidsOpen();
    brakes.depowerSLA();
    brakes.depowerSLR();

    DEBUG_PRINTLN( "Entered wait state" );
}

void waitUpdate()
{
    // keep accumulator pressurized
    //accumulator.maintainPressure();

    // TODO: Is this check needed? Don't we force transition elsewhere?
    if( pressure_req > ZERO_PRESSURE + .01 )
    {
        brakeStateMachine.transitionTo(Brake);
    }
}

void waitExit()
{
}
void brakeEnter() {
    // close master cylinder solenoids because they'll spill back to the reservoir
    smc.solenoidsClose();

    digitalWrite( PIN_BRAKE_SWITCH_1, HIGH );

    // close SLRRs, they are normally open for failsafe conditions
    brakes.depowerSLR();

    DEBUG_PRINTLN("entered brake state");
}

void brakeUpdate()
{
    // maintain accumulator pressure
    //accumulator.maintainPressure();

    // calculate a delta t
    lastMicros = currMicros;
    currMicros = micros();  // Fast loop, needs more precision than millis
    deltaT = currMicros - lastMicros;


    // take a reading from the brake pressure sensors
    brakes.updatePressure();
    pressure = ( brakes._pressureLeft + brakes._pressureRight ) / 2;


    pressureRate = ( pressure - pressure_last)/ deltaT;  // pressure/microsecond
    pressureRate_target = pressure_req - pressure;

    pressure_last = pressure;

    pidParams.derivative_gain = 0.50;
    pidParams.proportional_gain = 10.0;
    pidParams.integral_gain = 1.5;
    int ret = pid_update( &pidParams, pressureRate_target, pressureRate, 0.050 );

    if( ret == PID_SUCCESS )
    {
        double pressurePID_output = pidParams.control;

        // constrain to min/max
        pressurePID_output = m_constrain(
                (float) (pressurePID_output),
                (float) -2.0f,
                (float) 2.0f );

        Serial.print(" request = ");
        Serial.print(pressure_req); // Rate error
        Serial.print(" SR error = ");
        Serial.print(pressureRate_target - pressureRate); // Rate error
        Serial.print( " pressure = ");
        Serial.println(pressure);

        // some logic to set a samplerate for data which is sent to processing for plotting
        unsigned long currentMillis = millis();
        if ((unsigned long)(currentMillis - previousMillis) >= 100)
        {
            previousMillis = currentMillis;
        }


        // if pressure is too high
        if( pressurePID_output < -0.1 )
        {
            brakes.depowerSLA();
            brakes.powerSLR(calculateSLRDutyCycle(pressurePID_output));
        }

        // if pressure is too low
        if( pressurePID_output > 0.1 )
        {
            brakes.depowerSLR();
            brakes.powerSLA(calculateSLADutyCycle(pressurePID_output));
        }


        // if driver is not braking, transition to wait state
        if( pressure_req <= ZERO_PRESSURE)
        {
            DEBUG_PRINTLN("pressure request below threshold");
            brakeStateMachine.transitionTo( Wait );
        }
    }
}

void brakeExit()
{
    // close master cylinder solenoids
    smc.solenoidsOpen();

    // depower wheel solenoids to vent brake pressure at wheels
    brakes.depowerSLA();

    // unswitch brake switch
    digitalWrite( PIN_BRAKE_SWITCH_1, LOW );
}



// the setup routine runs once when you press reset:
void setup( void )
{

    // duty Scalers good for 0x05
    //SLADutyMax = 50;
    //SLADutyMin = 5;
    //SLRDutyMax = 50;
    //SLRDutyMin = 20;


    // duty Scalers good for 0x02
    //SLADutyMax = 225;
    //SLADutyMin = 100;
    //SLRDutyMax = 225;
    //SLRDutyMin = 100;

    // test duty Scalers
    SLADutyMax = 100;
    SLADutyMin = 0;
    SLRDutyMax = 100;
    SLRDutyMin = 0;

    // set the PWM timers, above the acoustic range
    TCCR3B = (TCCR3B & 0xF8) | 0x02; // pins 2,3,5 | timer 3
    TCCR4B = (TCCR4B & 0xF8) | 0x02; // pins 6,7,8 | timer 4


    /*
       0x01      31.374 KHz
       0x02      3.921 KHz
       0x03      980.3 Hz
       0x04      490.1 Hz
       0x05      245 hz
       0x06      122.5 hz
       0x07      30.63 hz
     */

    // zero
    last_update_ms = 0;
    memset( &rx_frame_ps_ctrl_brake_command, 0, sizeof(rx_frame_ps_ctrl_brake_command) );

    // relay boards are active low, set to high before setting output to avoid unintended energisation of relay
    digitalWrite( PIN_BRAKE_SWITCH_1, LOW );
    pinMode( PIN_BRAKE_SWITCH_1, OUTPUT );

    // depower all the things
    accumulator.pumpOff();
    smc.solenoidsOpen();

    // close rear slrs. These should open only for emergencies and to release brake pressure
    brakes.depowerSLR();
    brakes.depowerSLA();

    init_serial();

    init_can();

    //publish_ps_ctrl_brake_report();

    // update last Rx timestamps so we don't set timeout warnings on start up
    //rx_frame_ps_ctrl_brake_command.timestamp = GET_TIMESTAMP_MS();

    // update the global system update timestamp, ms
    //last_update_ms = GET_TIMESTAMP_MS();

    // Initialize PID params
    pid_zeroize( &pidParams,  BRAKE_PID_WINDUP_GUARD );

    // debug log
    DEBUG_PRINTLN( "init: pass" );

}

void loop()
{

    // update the global system update timestamp, ms
    //last_update_ms = GET_TIMESTAMP_MS();

    // check pressures on master cylinder (pressure from pedal)
    smc.checkPedal();

    accumulator.readPumpPressure();

    handle_ready_rx_frames();

    publish_timed_tx_frames();

    // read and parse incoming serial commands
    if( Serial.available() > 0 )
    {
        incomingSerialByte = Serial.read();
        processSerialByte();
    }

    brakeStateMachine.update();
}
