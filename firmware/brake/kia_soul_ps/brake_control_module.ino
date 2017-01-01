/************************************************************************/
/* Copyright (c) 2016 PolySync Technologies, Inc.  All Rights Reserved. */
/*                                                                      */
/* This file is part of Open Source Car Control (OSCC).                 */
/*                                                                      */
/* OSCC is free software: you can redistribute it and/or modify         */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or    */
/* (at your option) any later version.                                  */
/*                                                                      */
/* OSCC is distributed in the hope that it will be useful,              */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/*                                                                      */
/* You should have received a copy of the GNU General Public License    */
/* along with OSCC.  If not, see <http://www.gnu.org/licenses/>.        */
/************************************************************************/

// Throttle control ECU firmware
// Firmware for control of 2014 Kia Soul throttle system
// Component
//    Arduino Mega
//    Seeed Studio CAN-BUS Shield, v1.2 (MCP2515)
//    Sainsmart 4 relay module
//    6 channel mosfet Board
// E Livingston, 2016

#include <SPI.h>
#include <PID_v1.h>
#include <FiniteStateMachine.h>
#include "mcp_can.h"
#include "can_frame.h"
#include "control_protocol_can.h"

#define PSYNC_DEBUG_FLAG

// show us if debugging
#ifdef PSYNC_DEBUG_FLAG
#warning "PSYNC_DEBUG_FLAG defined"
#endif




// *****************************************************
// static global data
// *****************************************************

// chip select pin for CAN Shield
#define CAN_CS 10


//
#define CAN_CONTROL_BAUD (CAN_500KBPS)


//
#define SERIAL_DEBUG_BAUD (115200)


//
#define CAN_INIT_RETRY_DELAY (50)


#ifdef PSYNC_DEBUG_FLAG
    #define DEBUG_PRINT(x)  Serial.println(x)
#else
    #define DEBUG_PRINT(x)
#endif


// ms
#define PS_CTRL_RX_WARN_TIMEOUT (150)


//
#define GET_TIMESTAMP_MS() ((uint32_t) millis())


//
#define GET_TIMESTAMP_US() ((uint32_t) micros())




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





// *****************************************************
// static declarations
// *


// corrects for overflow condition
static void get_update_time_delta_ms(
		const uint32_t time_in,
		const uint32_t last_update_time_ms,
		uint32_t * const delta_out )
{
    // check for overflow
    if( last_update_time_ms < time_in )
    {   
		// time remainder, prior to the overflow
		(*delta_out) = (UINT32_MAX - time_in);
        
        // add time since zero
        (*delta_out) += last_update_time_ms;
    }   
    else
    {   
        // normal delta
        (*delta_out) = ( last_update_time_ms - time_in );
    }   
}


// uses last_update_ms, corrects for overflow condition
static void get_update_time_ms(
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
// Duty cycles of pins 3 and 5 controlled by timer 3 (TCCR3B)
const byte PIN_SLAFL = 3;      // front left actuation
const byte PIN_SLAFR = 5;      // front right actuation
// Duty cycles of pins 6, 7, and 8 controlled by timer 4 (TCCR4B)
const byte PIN_SLRFL = 6;      // front left return
const byte PIN_SLRFR = 7;      // front right return
const byte PIN_SMC = 8;      // master cylinder solenoids (two of them)

const byte PIN_PUMP = 9;     // accumulator pump motor


// brake spoofer relay pin definitions
const byte PIN_BREAK_SWITCH_1 = 48;
const byte PIN_BREAK_SWITCH_2 = 49;


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
const double PRESSURE_STEP = 0.2;         // The amount that the 'a' and 'd' commands change the
                                          // voltage each time they are pressed.
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

double pressurePID_input,
       pressurePID_output,
       pressurePID_setpoint;

float P_Kp=10.0, 
      P_Ki=1.5, 
      P_Kd=0.50;

// intialize PID
PID pressurePID(&pressurePID_input, &pressurePID_output, &pressurePID_setpoint, P_Kp, P_Ki, P_Kd, DIRECT);


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
  int scale =  map(scaled, 0, 1024, SLADutyMin, SLADutyMax);
  return scale;
}


int calculateSLRDutyCycle(float pre) {
  int scaled = abs(pre) * 512;
  int scale =  map(scaled, 0, 1024, SLRDutyMin, SLRDutyMax);
  return scale;
}

float pressureToVoltage(int MPa) {
    return ( MPa + 217.1319446 ) / 505.5662053;
    // convert MPa pressure to equivalent voltage
    return MPa;
}

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

    // maintain accumulator pressure
    void maintainPressure() 
    {
      _pressure = convertToVoltage(analogRead(_sensorPin));


      if( _pressure < MIN_PACC ) 
      {
          pumpOn();
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
        _pressure1 = convertToVoltage(analogRead(_sensor1Pin));
        _pressure2 = convertToVoltage(analogRead(_sensor2Pin));

        // if current pedal pressure is greater than limit (because of
        // driver override by pressing the brake pedal), disable.
        if (_pressure1 > PEDAL_THRESH || _pressure2 > PEDAL_THRESH ) 
        {
            DEBUG_PRINT("Brake Pedal Detected");
            pressure_req = ZERO_PRESSURE;
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
      _pressureLeft = convertToVoltage( analogRead(_sensorPinLeft) );
      _pressureRight = convertToVoltage( analogRead(_sensorPinRight) );
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
Brakes brakes = Brakes( PIN_PFL, PIN_PFL, PIN_SLAFL, PIN_SLAFR, PIN_SLRFL, PIN_SLRFR);



//
static void init_serial( void )
{
    // Disable serial
    Serial.end();

    // Init if debugging
#ifdef PSYNC_DEBUG_FLAG
    Serial.begin( SERIAL_DEBUG_BAUD );
#endif

    // Debug log
    DEBUG_PRINT( "init_serial: pass" );
}


//
static void init_can( void )
{
    // Wait until we have initialized
    while( CAN.begin(CAN_CONTROL_BAUD) != CAN_OK )
    {
        // wait a little
        delay( CAN_INIT_RETRY_DELAY );
    }

    // Debug log
    DEBUG_PRINT( "init_can: pass" );
}


// A function to parse incoming serial bytes
void processSerialByte() {
  
  if (incomingSerialByte == 'a') {                  // increase pressure
      pressure_req += PRESSURE_STEP;
    }
  if (incomingSerialByte == 'd') {                  // decrease pressure
      pressure_req -= PRESSURE_STEP;
    }

  if (incomingSerialByte == 'p') {                  // reset
      pressure_req = ZERO_PRESSURE;
      DEBUG_PRINT("reset pressure request");
    }
}



//
static void publish_ps_ctrl_brake_report( void )
{
    // cast data
    ps_ctrl_brake_report_msg * const data =
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
    get_update_time_ms( &tx_frame_ps_ctrl_brake_report.timestamp, &delta );

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

    rx_frame_ps_ctrl_brake_command.timestamp = GET_TIMESTAMP_MS();

    unsigned int pedal_command = control_data->pedal_command;
    pressure_req = map(pedal_command, 0, 65535, 48, 230); // map to voltage range 
    pressure_req = pressure_req / 100;
    DEBUG_PRINT("pressure_req: ");
    DEBUG_PRINT(pressure_req);
     

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
    float pressure = ( brakes._pressureLeft + brakes._pressureRight ) / 2;
    DEBUG_PRINT(pressure);
    DEBUG_PRINT(",");
    DEBUG_PRINT(brake_pressure);
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

    DEBUG_PRINT( "Entered wait state" );
}

void waitUpdate() 
{
    // keep accumulator pressurized
    accumulator.maintainPressure();

    // TODO: Is this check needed? Don't we force transition elsewhere?
    if( pressure_req > ZERO_PRESSURE + .01 && controlEnabled ) 
    {
        brakeStateMachine.transitionTo(Brake);
    }
}

void waitExit() 
{
}

void brakeEnter()
{
    // close master cylinder solenoids because they'll spill back to the reservoir
    smc.solenoidsClose(); 

    digitalWrite( PIN_BREAK_SWITCH_1, LOW );
    digitalWrite( PIN_BREAK_SWITCH_2, LOW );

    // close SLRRs, they are normally open for failsafe conditions
    brakes.depowerSLR(); 

    DEBUG_PRINT("entered brake state");
}

void brakeUpdate() 
{
    // maintain accumulator pressure
    accumulator.maintainPressure();

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

    pressurePID_input = pressureRate;
    pressurePID_setpoint = pressureRate_target;
    pressurePID.SetTunings(P_Kp, P_Ki, P_Kd);

    pressurePID.Compute();



    // lots of PID debugging prints
    //Serial.print("Kp = ");
    //Serial.print(pressurePID.GetKp());  
    //Serial.print(" Ki = ");
    //Serial.print(pressurePID.GetKi()); 
    //Serial.print(" Kd = ");
    //Serial.print(pressurePID.GetKd()); 
    //Serial.print(" SR error = ");
    //Serial.print(pressureRate_target - pressureRate); // Rate error
    //Serial.print(" Commanded rate = ");
    //Serial.print(pressurePID_output);
    ////Serial.print( "deltaT = ");
    ////Serial.println(deltaT);
    //Serial.print( " pressure = ");
    //Serial.println(pressure);



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
        DEBUG_PRINT("pressure request below threshold");
        brakeStateMachine.transitionTo( Wait );
    }
}

void brakeExit() 
{
    // close master cylinder solenoids
    smc.solenoidsOpen(); 

    // depower wheel solenoids to vent brake pressure at wheels
    brakes.depowerSLA();

    // unswitch brake switch
    digitalWrite( PIN_BREAK_SWITCH_1, HIGH );
    digitalWrite( PIN_BREAK_SWITCH_2, HIGH );
}


//
static void check_rx_timeouts( void )
{
    // local vars
    uint32_t delta = 0;

    // get time since last receive
    get_update_time_delta_ms( 
			rx_frame_ps_ctrl_brake_command.timestamp, 
			GET_TIMESTAMP_MS(), 
			&delta );

    // check rx timeout
    if( delta >= PS_CTRL_RX_WARN_TIMEOUT ) 
    {
        // disable control from the PolySync interface
        if( controlEnabled ) 
        {
            Serial.println("control disabled: timeout");
            controlEnabled = false;
        }
    }
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
    SLADutyMax = 225;
    SLADutyMin = 100;
    SLRDutyMax = 225;
    SLRDutyMin = 100;

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
    digitalWrite( PIN_BREAK_SWITCH_1, HIGH );
    digitalWrite( PIN_BREAK_SWITCH_2, HIGH );
    pinMode( PIN_BREAK_SWITCH_1, OUTPUT );
    pinMode( PIN_BREAK_SWITCH_2, OUTPUT );

    // depower all the things
    accumulator.pumpOff();
    smc.solenoidsOpen();

    // close rear slrs. These should open only for emergencies and to release break pressure
    brakes.depowerSLR();
    brakes.depowerSLA();

    init_serial();

    init_can();

    publish_ps_ctrl_brake_report();

    // update last Rx timestamps so we don't set timeout warnings on start up
    rx_frame_ps_ctrl_brake_command.timestamp = GET_TIMESTAMP_MS();

    // update the global system update timestamp, ms
    last_update_ms = GET_TIMESTAMP_MS();

    pressurePID.SetMode(AUTOMATIC);
    pressurePID.SetOutputLimits(-2, 2);
    pressurePID.SetSampleTime(50);

    // debug log
    DEBUG_PRINT( "init: pass" );

}

void loop() 
{

    // update the global system update timestamp, ms
    last_update_ms = GET_TIMESTAMP_MS();

    handle_ready_rx_frames();

    publish_timed_tx_frames();

    check_rx_timeouts();
        
    // check pressures on master cylinder (pressure from pedal)
    smc.checkPedal();

    // read and parse incoming serial commands
    if( Serial.available() > 0 ) 
    {
        incomingSerialByte = Serial.read();
        processSerialByte();
    }
      

    brakeStateMachine.update();
} 
