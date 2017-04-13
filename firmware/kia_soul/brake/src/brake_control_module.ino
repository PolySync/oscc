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

// Brake control ECU firmware
// 2004-2009 Prius brake actuator


#include <SPI.h>
#include "FiniteStateMachine.h"
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
    #define DEBUG_PRINT(x)  Serial.println(x)
#else
    #define DEBUG_PRINT(x)
#endif

// chip select pin for CAN Shield
#define CAN_CS 53

// ms
#define PS_CTRL_RX_WARN_TIMEOUT ( 250 )

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
const byte PIN_SLAFL = 5;      // front left actuation
const byte PIN_SLAFR = 7;      // front right actuation

// Duty cycles of pins 6, 7, and 8 controlled by timer 4 (TCCR4B)
const byte PIN_SLRFL = 6;      // front left return
const byte PIN_SLRFR = 8;      // front right return
const byte PIN_SMC   = 2;      // master cylinder solenoids (two of them)

const byte PIN_BRAKE_SWITCH = 48;
const byte PIN_PUMP         = 49;     // accumulator pump motor

// sensor pin (analog) definitions
const byte PIN_PACC = 9;       // pressure accumulator sensor
const byte PIN_PMC1 = 10;      // pressure master cylinder sensor 1
const byte PIN_PMC2 = 11;      // pressure master cylinder sensor 2
const byte PIN_PFR  = 13;      // pressure front right sensor
const byte PIN_PFL  = 14;      // pressure front left sensor


// the following are guesses, these need to be debugged/researched
const double ZERO_PRESSURE = 0.48;        // The voltage the sensors read when no pressure is present
const double MIN_PACC = 2.3;              // minumum accumulator pressure to maintain
const double MAX_PACC = 2.4;              // max accumulator pressure to maintain
const double PEDAL_THRESH = 0.6;          // Pressure for pedal interference

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

unsigned int pedal_command_raw;
int brake_pressure_can;

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

    // *****************************************************
    // Function:    maintainPressure()
    //
    // Purpose:     This function checks the voltage input from the accumulator
    //              pressure sensor to determine if the accumulator pump should
    //              be powered on or powered off. The accumulator should maintain
    //              enough pressure to emergency brake at any point.
    //
    //              Because analog voltage sensors are being read, a filter is applied
    //              to the reading to ensure that voltage drops/spikes don't effect
    //              the reading.
    //
    //
    // Returns:     void
    //
    // Parameters:  None
    //
    // *****************************************************
    void maintainPressure()
    {
        static const float filter_alpha = 0.05;
        static float _pressure = 0.0;

        // This is going to get filtered
        float sensor_1 = convertToVoltage(analogRead(_sensorPin));

        _pressure = ( filter_alpha * sensor_1 ) +
            ( ( 1.0 - filter_alpha ) * _pressure );

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

    // *****************************************************
    // Function:    check_brake_pedal
    //
    // Purpose:     This function checks the voltage input from the brake pedal
    //              sensors to determine if the driver is attempting to brake
    //              the vehicle.  This must be done over time by taking
    //              periodic samples of the input voltage, calculating the
    //              difference between the two and then passing that difference
    //              through a basic exponential filter to smooth the input.
    //
    //              The required response time for the filter is 250 ms, which at
    //              50ms per sample is 5 samples.  As such, the alpha for the
    //              exponential filter is 0.5 to make the input go "close to" zero
    //              in 5 samples.
    //
    //              The implementation is:
    //                  s(t) = ( a * x(t) ) + ( ( 1 - a ) * s ( t - 1 ) )
    //
    //              If the filtered input exceeds the max voltage, it is an
    //              indicator that the driver is pressing on the brake pedal
    //              and the control should be disabled.
    //
    // Returns:     void
    //
    // Parameters:  None
    //
    // *****************************************************
    void check_brake_pedal( )
    {
        static const float filter_alpha = 0.05;
        static const float max_pedal_voltage = PEDAL_THRESH;

        static float filtered_input_1 = 0.0;
        static float filtered_input_2 = 0.0;

        float sensor_1 = ( float )( analogRead( _sensor1Pin ) );
        float sensor_2 = ( float )( analogRead( _sensor2Pin ) );

        // Convert the input to be on a 5V scale
        sensor_1 *= ( 5.0 / 1023.0 );
        sensor_2 *= ( 5.0 / 1023.0 );

        filtered_input_1 = ( filter_alpha * sensor_1 ) +
                                ( ( 1.0 - filter_alpha ) * filtered_input_1 );

        filtered_input_2 = ( filter_alpha * sensor_2 ) +
                                ( ( 1.0 - filter_alpha ) * filtered_input_2 );

        if ( ( filtered_input_1 > max_pedal_voltage ) ||
             ( filtered_input_2 > max_pedal_voltage ) )
        {
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
        digitalWrite( _solenoidPinLeftR, 0 );
        digitalWrite( _solenoidPinRightR, 0 );
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
Brakes brakes = Brakes( PIN_PFL, PIN_PFR, PIN_SLAFL, PIN_SLAFR, PIN_SLRFL, PIN_SLRFR);

//
static void init_serial( void )
{
    Serial.begin( SERIAL_BAUD );

    // debug log
    DEBUG_PRINT( "init_serial: pass" );
}


//
static void init_can( void )
{
    // Wait until we have initialized
    while( CAN.begin( CAN_BAUD ) != CAN_OK )
    {
        // wait a little
        delay( CAN_INIT_RETRY_DELAY );
        DEBUG_PRINT( "init_can: retrying" );
    }

    // Debug log
    DEBUG_PRINT( "init_can: pass" );
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

    // Set enabled flag
    data->enabled = (uint8_t) controlEnabled;

    // Set pedal input
    data->pedal_input = brake_pressure_can;

    // Set pedal command
    data->pedal_command = pedal_command_raw;

    // Set pedal command
    data->pedal_output = pressure;

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

    // enable control from the high-level interface
    if( enabled == 1 && !controlEnabled )
    {
        controlEnabled = true;
        brakeStateMachine.transitionTo(Brake);
    }

    // disable control from the high-level interface
    if( enabled == 0 && controlEnabled )
    {
        controlEnabled = false;
        brakeStateMachine.transitionTo(Wait);
    }

    rx_frame_ps_ctrl_brake_command.timestamp = GET_TIMESTAMP_MS();

    pedal_command_raw = control_data->pedal_command;
    pressure_req = map(pedal_command_raw, 0, 65535, 48, 230); // map to voltage range
    pressure_req = pressure_req / 100;
}

static void process_psvc_chassis_state1( const uint8_t * const rx_frame_buffer )
{
    const psvc_chassis_state1_data_s * chassis_data =
        (psvc_chassis_state1_data_s*) rx_frame_buffer;

    // brake pressure as reported from the C-CAN bus
    brake_pressure_can = chassis_data->brake_pressure;

    // take a reading from the brake pressure sensors
    brakes.updatePressure();

    // average the pressure of the rear and front lines
    float pressure = ( brakes._pressureLeft + brakes._pressureRight ) / 2;
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

    // close SLRRs, they are normally open for failsafe conditions
    brakes.depowerSLR();
    DEBUG_PRINT("entered brake state");
}


//
void brakeUpdate()
{
    // maintain accumulator pressure
    accumulator.maintainPressure();

    // take a reading from the brake pressure sensors
    brakes.updatePressure();
    pressure = ( brakes._pressureLeft + brakes._pressureRight ) / 2;

    if (pressure_req > ZERO_PRESSURE )
    {

/*******************************************************************************
*   WARNING
*
*   The ranges selected to do brake control are carefully tested to ensure that
*   the pressure actuated is not outside of the range of what the brake module
*   can handle. By changing any of this code you risk attempting to actuate
*   a pressure outside of the brake modules valid range. Actuating a pressure
*   outside of the modules valid range will, at best, cause it to go into an
*   unrecoverable fault state. This is characterized by the accumulator
*   "continuously pumping" without accumulating any actual pressure, or being
*   "over pressured." Clearing this fault state requires expert knowledge of the
*   breaking module.
*
*   It is NOT recommended to modify any of the existing control ranges, or
*   gains, without expert knowledge.
*******************************************************************************/

        digitalWrite( PIN_BRAKE_SWITCH, HIGH );
        smc.solenoidsClose();
        // calculate a delta t
        lastMicros = currMicros;
        currMicros = micros();  // Fast loop, needs more precision than millis
        deltaT = currMicros - lastMicros;

        pressureRate = ( pressure - pressure_last)/ deltaT;  // pressure/microsecond
        pressureRate_target = pressure_req - pressure;

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



        }
    }
    else if( pressure_req <= ZERO_PRESSURE )
    {
        smc.solenoidsOpen();
        brakes.depowerSLA();
        brakes.depowerSLR();

        // unswitch brake switch
        digitalWrite( PIN_BRAKE_SWITCH, LOW );
    }
}


//
void brakeExit()
{
    // close master cylinder solenoids
    smc.solenoidsOpen();

    // depower wheel solenoids to vent brake pressure at wheels
    brakes.depowerSLA();

    // unswitch brake switch
    digitalWrite( PIN_BRAKE_SWITCH, LOW );
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
            controlEnabled = false;
            brakeStateMachine.transitionTo(Wait);
        }
    }
}


// the setup routine runs once when you press reset:
void setup( void )
{
    // set the Arduino's PWM timers to 3.921 KHz, above the acoustic range
    TCCR3B = (TCCR3B & 0xF8) | 0x02; // pins 2,3,5 | timer 3
    TCCR4B = (TCCR4B & 0xF8) | 0x02; // pins 6,7,8 | timer 4

    // set the min/max duty cycle scalers used for 3.921 KHz PWM frequency.
    // These represent the minimum duty cycles that begin to actuate the proportional solenoids
    // and the maximum dudty cycle where the solenoids have reached their stops.
    SLADutyMax = 105;
    SLADutyMin = 50;
    SLRDutyMax = 100;
    SLRDutyMin = 50;

    // zero
    last_update_ms = 0;
    memset( &rx_frame_ps_ctrl_brake_command, 0, sizeof(rx_frame_ps_ctrl_brake_command) );

    digitalWrite( PIN_BRAKE_SWITCH, LOW );
    pinMode( PIN_BRAKE_SWITCH, OUTPUT );

    // depower all the things
    accumulator.pumpOff();
    smc.solenoidsOpen();

    // close rear slrs. These should open only for emergencies and to release brake pressure
    brakes.depowerSLR();

    // Clear any pressure in the accumulator
    brakes.powerSLA(250);
    delay(3000);

    // initialize for braking
    brakes.depowerSLA();

    init_serial();

    init_can();

    publish_ps_ctrl_brake_report();

    // update last Rx timestamps so we don't set timeout warnings on start up
    rx_frame_ps_ctrl_brake_command.timestamp = GET_TIMESTAMP_MS();

    // update the global system update timestamp, ms
    last_update_ms = GET_TIMESTAMP_MS();

    // Initialize PID params
    pid_zeroize( &pidParams, BRAKE_PID_WINDUP_GUARD );

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
    smc.check_brake_pedal();

    brakeStateMachine.update();
}
