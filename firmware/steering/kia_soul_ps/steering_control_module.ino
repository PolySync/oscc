// Steering control ECU firmware
// Firmware for control of 2014 Kia Soul Motor Driven Power Steering (MDPS) system
// Components:
//    Arduino Uno
//    Seeed Studio CAN-BUS Shield, v1.2 (MCP2515)
//    Sainsmart 4 relay module
//    ETT ET-MINI SPI DAC (MCP4922)
// J Hartung, 2015




#include <SPI.h>

#include "mcp_can.h"
#include "can_frame.h"
#include "control_protocol_can.h"
#include "current_control_state.h"
#include "PID.h"




// *****************************************************
// static global types/macros
// *****************************************************

#define PSYNC_DEBUG_FLAG

//
#ifdef PSYNC_DEBUG_FLAG
    #define DEBUG_PRINT(x)  Serial.println(x)
#else
    #define DEBUG_PRINT(x)
#endif

// Set CAN_CS to pin 9 for CAN
#define CAN_CS 9

#define CAN_BAUD ( CAN_500KBPS )

//
#define SERIAL_BAUD (115200)

//
#define CAN_INIT_RETRY_DELAY (50)

//
#define GET_TIMESTAMP_MS() ((uint32_t) millis())

// ms
#define PS_CTRL_RX_WARN_TIMEOUT (200) //(50)


// Set up pins for interface with the ET-MINI DAV (MCP4922)
#define SHDN                12  // Shutdown

#define LDAC                8   // Load data

#define DAC_CS              10  // Chip select pin

#define DAC_PWR             A5  // Power the DAC from the Arduino digital pins

#define TSENS_LOW           A1  // Green wire from the torque sensor, low values

#define TSENS_LOW_SPOOF     A0  // Sensing input for the DAC output

#define TSENS_HIGH          A3  // Blue wire from the torque sensor, high values

#define TSENS_HIGH_SPOOF    A2  // Sensing input for the DAC output

#define TSENS_LOW_SIGINT    6   // Signal interrupt (relay) for low torque values (blue wire)

#define TSENS_HIGH_SIGINT   7   // Signal interrupt (relay) for high torque values (green wire)

#define STEERING_WHEEL_CUTOFF_THRESHOLD 3000




// *****************************************************
// static structures
// *****************************************************


// Construct the CAN shield object
MCP_CAN CAN(CAN_CS);                                    // Set CS pin for the CAN shield


//
static can_frame_s rx_frame_ps_ctrl_steering_command;


//
static can_frame_s tx_frame_ps_ctrl_steering_report;


//
static current_control_state current_ctrl_state;


//
static PID pidParams;


// *****************************************************
// non-static global veriables
// *****************************************************


const int numReadings = 2;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total


// *****************************************************
// static declarations
// *****************************************************


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


//
static void init_serial( void ) 
{
    Serial.begin( SERIAL_BAUD );
}


//
static void init_can ( void ) 
{
    // wait until we have initialized
    while( CAN.begin(CAN_BAUD) != CAN_OK )
    {   
        // wait a little
        delay( CAN_INIT_RETRY_DELAY );
    }   

    // debug log
    DEBUG_PRINT( "init_obd_can: pass" );
}




/* ====================================== */
/* ============== CONTROL =============== */
/* ====================================== */


// A function to set the DAC output registers
void setDAC(uint16_t data, char channel) 
{
	uint8_t message[2];

	if (channel == 'A') 
	{
		data |= 0x1000; // Set DAC A enable

		// Load data into 8 byte payloads
		message[0] = (data >> 8) & 0xFF;
		message[1] = data & 0xFF;
		
		digitalWrite(DAC_CS, LOW);  // Select the DAC for SPI transfer

		// Transfer the data payload over SPI
		SPI.transfer(message[0]);
		SPI.transfer(message[1]);

		// Relinquish SPI control
		digitalWrite(DAC_CS, HIGH);
  	}
  
  	else if (channel == 'B') 
	{
		data |= 0x9000;  // Set DAC B enable
		
		// Load data into 8 byte payloads
		message[0] = (data >> 8) & 0xFF;
		message[1] = data & 0xFF;
		
		digitalWrite(DAC_CS, LOW);  // Select the DAC for SPI transfer

		// Transfer the data payload over SPI
		SPI.transfer(message[0]);
		SPI.transfer(message[1]);

		// Relinquish SPI control ASAP
		digitalWrite(DAC_CS, HIGH);
  	}
}


// A function to set the DAC output
void latchDAC() {
  
  // Pulse the LDAC line to send registers to DAC out. Must have set DAC registers with setDAC() first for this to do anything.
  digitalWrite( LDAC, LOW );

  delayMicroseconds( 50 );

  digitalWrite( LDAC, HIGH );  
}


// A function to enable SCM to take control
void enableControl() 
{
	// Do a quick average to smooth out the noisy data
	static int AVG_max = 20;  // Total number of samples to average over
	long readingsL = 0;
	long readingsH = 0;

	for (int i = 0; i < AVG_max; i++) 
	{
		readingsL += analogRead(TSENS_LOW);
		readingsH += analogRead(TSENS_HIGH);
	}

	uint16_t TSensL_current = (readingsL / AVG_max) << 2;
	uint16_t TSensH_current = (readingsH / AVG_max) << 2;

	// Write measured torque values to DAC to avoid a signal discontinuity when the SCM takes over
	setDAC(TSensH_current, 'A');
	setDAC(TSensL_current, 'B');
	latchDAC();

	// TODO: check if the DAC value and the sensed values are the same. If not, return an error and do NOT enable the sigint relays.

	// Enable the signal interrupt relays
	digitalWrite(TSENS_LOW_SIGINT, LOW);
	digitalWrite(TSENS_HIGH_SIGINT, LOW);

	current_ctrl_state.control_enabled = true;

	DEBUG_PRINT("Control enabled");
}


// A function to disable SCM control
void disableControl() {
  
	// Do a quick average to smooth out the noisy data
	static int AVG_max = 20;  // Total number of samples to average over
	long readingsL = 0;
	long readingsH = 0;

	for (int i = 0; i < AVG_max; i++) 
	{
		readingsL += analogRead(TSENS_LOW) << 2;
		readingsH += analogRead(TSENS_HIGH) << 2;
	}

	uint16_t TSensL_current = readingsL / AVG_max;
	uint16_t TSensH_current = readingsH / AVG_max;

	// Write measured torque values to DAC to avoid a signal discontinuity when the SCM relinquishes control
	setDAC(TSensH_current, 'A');
	setDAC(TSensL_current, 'B');
	latchDAC();

	// Disable the signal interrupt relays
	digitalWrite(TSENS_LOW_SIGINT, HIGH);
	digitalWrite(TSENS_HIGH_SIGINT, HIGH);

	current_ctrl_state.control_enabled = false;

	DEBUG_PRINT("Control disabled");
}


//
void calculateTorqueSpoof( float torque, uint16_t * TSpoofL, uint16_t * TSpoofH ) 
{
	// Values calculated with min/max calibration curve and hand tuned for neutral balance
	// DAC requires 12-bit values, (4096steps/5V = 819.2 steps/V)
	( *TSpoofL ) = 819.2*( 0.0008*torque + 2.26 );
	( *TSpoofH ) = 819.2*( -0.0008*torque + 2.5 );
}



/* ====================================== */
/* =========== COMMUNICATIONS =========== */
/* ====================================== */

// A function to parse incoming serial bytes
void processSerialByte( uint8_t incomingSerialByte) 
{
/*
    // enable/disable control
    if( incomingSerialByte == 'p' )                  
    {
        current_ctrl_state.emergency_stop = !current_ctrl_state.emergency_stop;

        disableControl();
    }

    if( incomingSerialByte == 'i' )
    {
        current_ctrl_state.SA_Kp += 0.001;
        Serial.print( "proportional gain increased: " );
        Serial.println( current_ctrl_state.SA_Kp );
    }
    if( incomingSerialByte == 'u' )
    {
        if( current_ctrl_state.SA_Kp > 0 )
        {
            current_ctrl_state.SA_Kp -= 0.001;
            Serial.print( "proportional gain decreased: " );
            Serial.println( current_ctrl_state.SA_Kp );
        }
    }

    if( incomingSerialByte == 'k' )
    {
        current_ctrl_state.SA_Ki += 0.01;
        Serial.print( "integral gain increased: " );
        Serial.println( current_ctrl_state.SA_Ki );
    }
    if( incomingSerialByte == 'j' )
    {
        if( current_ctrl_state.SA_Ki > 0 )
        {
            current_ctrl_state.SA_Ki -= 0.01;
            Serial.print( "integral gain decreased: " );
            Serial.println( current_ctrl_state.SA_Ki );
        }
    }

    if( incomingSerialByte == 'm' )
    {
        current_ctrl_state.SA_Kd += 0.0001;
        Serial.print( "derivative gain increased: " );
        Serial.println( current_ctrl_state.SA_Kd );
    }
    if( incomingSerialByte == 'n' )
    {
        if( current_ctrl_state.SA_Kd > 0 )
        {
            current_ctrl_state.SA_Kd -= 0.0001;
            Serial.print( "derivative gain decreased: " );
            Serial.println( current_ctrl_state.SA_Kd );
        }
    }
*/
}


//
static void publish_ps_ctrl_steering_report( void )
{
    // cast data
    ps_ctrl_steering_report_msg * const data =
            (ps_ctrl_steering_report_msg*) tx_frame_ps_ctrl_steering_report.data;

    // Set frame ID
    tx_frame_ps_ctrl_steering_report.id = (uint32_t) (PS_CTRL_MSG_ID_STEERING_REPORT);

    // Set DLC
    tx_frame_ps_ctrl_steering_report.dlc = 8; 

    // Steering Wheel Angle
    data->angle = current_ctrl_state.current_steering_angle;

    // Update last publish timestamp, ms
    tx_frame_ps_ctrl_steering_report.timestamp = GET_TIMESTAMP_MS();
    
    // Publish to control CAN bus
    CAN.sendMsgBuf(
            tx_frame_ps_ctrl_steering_report.id,
            0, // standard ID (not extended)
            tx_frame_ps_ctrl_steering_report.dlc,
            tx_frame_ps_ctrl_steering_report.data );
}   


//  
static void publish_timed_tx_frames( void )
{
    // Local vars
    uint32_t delta = 0;

    // Get time since last publish
    get_update_time_delta_ms( 
			tx_frame_ps_ctrl_steering_report.timestamp, 
			GET_TIMESTAMP_MS(),
			&delta );

    // check publish interval
    if( delta >= PS_CTRL_STEERING_REPORT_PUBLISH_INTERVAL )
    {
        // publish frame, update timestamp
        publish_ps_ctrl_steering_report();
    }
}


//
static void process_ps_ctrl_steering_command( const uint8_t * const rx_frame_buffer )
{
    // Cast control frame data
    const ps_ctrl_steering_command_msg * const control_data =
            (ps_ctrl_steering_command_msg*) rx_frame_buffer;

    current_ctrl_state.commanded_steering_angle = control_data->steering_wheel_angle_command / 9.0 ;

    current_ctrl_state.steering_angle_rate_max = control_data->steering_wheel_max_velocity * 9.0;

    bool enabled = control_data->enabled == 1;

    // Enable control from the PolSync interface
    if( enabled == 1 && !current_ctrl_state.control_enabled && !current_ctrl_state.emergency_stop ) 
    {
        enableControl();
    }

    // Disable control from the PolySync interface
    if( enabled == 0 && current_ctrl_state.control_enabled )
    {
        disableControl();
    }

	rx_frame_ps_ctrl_steering_command.timestamp = GET_TIMESTAMP_MS();
}


//
static void process_psvc_chassis_state1( const uint8_t * const rx_frame_buffer )
{
    const psvc_chassis_state1_data_s * const chassis_data =
        (psvc_chassis_state1_data_s*) rx_frame_buffer;

    int16_t raw_angle = chassis_data->steering_wheel_angle;
    current_ctrl_state.current_steering_angle = float( raw_angle ) * 0.0076294;

	// Convert from 40 degree range to 470 degree range in 1 deg increments
	current_ctrl_state.current_steering_angle *= 11.7;

}


// A function to parse CAN data into useful variables
void handle_ready_rx_frames(void) 
{
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

        // check for a supported frame ID
        if( rx_frame.id == PS_CTRL_MSG_ID_STEERING_COMMAND )
        {
            // process steering commmand
            process_ps_ctrl_steering_command( rx_frame.data );
        }


        if( rx_frame.id == KIA_STATUS1_MESSAGE_ID )
        {
            // process state1
            process_psvc_chassis_state1( rx_frame.data );

        }
    }
}


//
static void check_rx_timeouts( void )
{
    // local vars
    uint32_t delta = 0;

    // get time since last receive
    get_update_time_delta_ms( 
			rx_frame_ps_ctrl_steering_command.timestamp, 
			GET_TIMESTAMP_MS(), 
			&delta );

    // check rx timeout
    if( delta >= PS_CTRL_RX_WARN_TIMEOUT ) 
    {
        // disable control from the PolySync interface
        if( current_ctrl_state.control_enabled ) 
        {
            disableControl();
        }
    }
}




/* ====================================== */
/* ================ SETUP =============== */
/* ====================================== */


//
void setup() 
{
    // zero
    memset( &rx_frame_ps_ctrl_steering_command, 0, sizeof(rx_frame_ps_ctrl_steering_command) );

    // Set up pin modes
    pinMode(SHDN, OUTPUT);
    pinMode(LDAC, OUTPUT);
    pinMode(DAC_CS, OUTPUT);
    pinMode(DAC_PWR, OUTPUT);
    pinMode(TSENS_LOW, INPUT);
    pinMode(TSENS_LOW_SPOOF, INPUT);
    pinMode(TSENS_HIGH, INPUT);
    pinMode(TSENS_HIGH_SPOOF, INPUT);
    pinMode(TSENS_LOW_SIGINT, OUTPUT);
    pinMode(TSENS_HIGH_SIGINT, OUTPUT);

    // Initialize the DAC board
    digitalWrite(DAC_PWR, HIGH);    // Supply power
    digitalWrite(DAC_CS, HIGH);     // Deselect DAC CS
    digitalWrite(SHDN, HIGH);       // Turn on the DAC
    digitalWrite(LDAC, HIGH);       // Reset data

    // Initialize relay board
    digitalWrite(TSENS_LOW_SIGINT, HIGH);
    digitalWrite(TSENS_HIGH_SIGINT, HIGH);

    init_serial();

    init_can();

    publish_ps_ctrl_steering_report();

	current_ctrl_state.control_enabled = false;

	current_ctrl_state.emergency_stop = false;

    // update last Rx timestamps so we don't set timeout warnings on start up
    rx_frame_ps_ctrl_steering_command.timestamp = GET_TIMESTAMP_MS();
    
    pid_zeroize( &pidParams );

    // debug log
    DEBUG_PRINT( "init: pass" );
}




/* ====================================== */
/* ================ LOOP ================ */
/* ====================================== */


//
void loop()
{
    handle_ready_rx_frames();

    publish_timed_tx_frames();

    check_rx_timeouts();

    // Read and parse incoming serial commands
    if ( Serial.available() > 0 ) 
	{
        uint8_t incomingSerialByte = Serial.read();
        processSerialByte( incomingSerialByte );
    }


    // Calculate a delta t
    long unsigned int currMicros = micros();  // Fast loop, needs more precision than millis
    unsigned int deltaT = currMicros - current_ctrl_state.lastMicros;


    if( deltaT > 50000 )
    {
        current_ctrl_state.lastMicros = currMicros;        

        // Now that we've set control status, do steering if we are in control
        if ( current_ctrl_state.control_enabled ) 
	    {
            // Calculate steering angle rates
            double steeringAngleRate = ( current_ctrl_state.current_steering_angle - current_ctrl_state.steering_angle_last )/0.05;  //  degree/microsecond
            double steeringAngleRateTarget = ( current_ctrl_state.commanded_steering_angle - current_ctrl_state.current_steering_angle )/0.05; //  degree/microsecond


            // subtract the last reading:
            total = total - readings[readIndex];
            readings[readIndex] = steeringAngleRate;
            total = total + readings[readIndex];
            readIndex = readIndex + 1;
            if (readIndex >= numReadings) {
                readIndex = 0;
            }
            // calculate the average:
            double average = total / numReadings;


            current_ctrl_state.steering_angle_last = current_ctrl_state.current_steering_angle;   // Remember for next time

            // Set saturation limits for steering wheel rotation speed
            if ( steeringAngleRateTarget >= current_ctrl_state.steering_angle_rate_max )
            {
                steeringAngleRateTarget = current_ctrl_state.steering_angle_rate_max;
            }

            if ( steeringAngleRateTarget <= -current_ctrl_state.steering_angle_rate_max ) 
            {
                steeringAngleRateTarget = -current_ctrl_state.steering_angle_rate_max;
            }

            current_ctrl_state.PID_input = average;
            current_ctrl_state.PID_setpoint = steeringAngleRateTarget;

            pidParams.derivative_gain = current_ctrl_state.SA_Kd;
            pidParams.proportional_gain = current_ctrl_state.SA_Kp;
            pidParams.integral_gain = current_ctrl_state.SA_Ki;

            pid_update( &pidParams, steeringAngleRateTarget - steeringAngleRate, 0.050 );

            uint16_t TSpoofH;
            uint16_t TSpoofL;

            double control = pidParams.control;

            // constrain to min/max
            control = m_constrain(
                    (float) (control),
                    (float) -1500.0f,
                    (float) 1500.0f );

            calculateTorqueSpoof( control, &TSpoofL, &TSpoofH );

            setDAC( TSpoofH, 'A' );
            setDAC( TSpoofL, 'B' );
            latchDAC();

        }
        else
        {
            pid_zeroize( &pidParams );

            total = total - readings[readIndex];
            readings[readIndex] = 0;
            total = total + readings[readIndex];
            readIndex = readIndex + 1;
            if (readIndex >= numReadings) 
            {
                readIndex = 0;
            }
        }

    }
}
