#include "SPI.h"
#include "mcp_can.h"
#include "can_frame.h"
#include "DAC_MCP49xx.h"
#include "common.h"



#define SIGNAL_INPUT_A A0     // input pin for sensing sensor 1 output

#define SIGNAL_INPUT_B A1     // input pin for sensing sensor 2 output

#define SPOOF_SIGNAL_A A2     // input pin for sensing DAC 1 output

#define SPOOF_SIGNAL_B A3     // input pin for sensing DAC 2 output

#define SPOOF_ENGAGE 6        // signal interrupt (relay) for spoofed signals

#define DAC_CS 9              // chip select pin for DAC

#define CAN_CS 10             // chip select pin for CAN


DAC_MCP49xx dac( DAC_MCP49xx::MCP4922, 9 ); // DAC model, SS pin, LDAC pin

// Construct the CAN shield object
MCP_CAN CAN( CAN_CS ); 


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



//BENCH TESTS


// the two DACS have circuitry for measuring the output. Create a signal and
// measure that the output is what is expected.
void test_DACS( ) 
{
    uint32_t dac_value;
    int spoof_signal_a_read = 0;
    int spoof_signal_b_read = 0;
    float spoof_a_voltage_read = 0;
    float spoof_b_voltage_read = 0;

    float dac_expected_output;

    // energize the relay so we can read the values at the terminal
    digitalWrite( SPOOF_ENGAGE, HIGH );

    for ( dac_value = 0; dac_value < 4096; dac_value = dac_value + 15 )
    {
        dac_expected_output = ( 5.0 / 4096.0 ) * dac_value;

        dac.outputA( dac_value );
        dac.outputB( dac_value );

        delay( 2000 );

        spoof_signal_a_read = analogRead( SPOOF_SIGNAL_A );
        spoof_signal_b_read = analogRead( SPOOF_SIGNAL_B );

        spoof_a_voltage_read = ( spoof_signal_a_read * 5.0 ) / 1024.0;
        spoof_b_voltage_read = ( spoof_signal_b_read * 5.0 ) / 1024.0;

        Serial.print( "DAC Value: " );
        Serial.print( dac_value );

        Serial.print( "\t\t\t\t\tExpected Voltage: " );
        Serial.println( dac_expected_output,3 );

        Serial.print( "Spoof A Value: " );
        Serial.print( spoof_signal_a_read );

        Serial.print( "\tSpoof B Value: " );
        Serial.print( spoof_signal_b_read );

        Serial.print( "\tSpoof A Voltage: " );      
        Serial.print( spoof_a_voltage_read, 3 );      

        Serial.print( "\tSpoof B Voltage: " );      
        Serial.println( spoof_b_voltage_read, 3 );  

        Serial.println( "" );    
    }    
}

// to test that the signal interrupt relay functions we can blink two
// LEDS in an alternating pattering using the the switch relay.
void test_interrupt_relay( ) 
{
    Serial.flush(); 

    while( !Serial.available() )
    {
        digitalWrite( SPOOF_ENGAGE, LOW );       // start off with the relay in it's depowered state
        delay( 500 );                            // wait half a second

        digitalWrite( SPOOF_ENGAGE, HIGH );      // energize the relay
        delay( 500 );                            // wait half a second
    }
}


// send a CAN frame, to be recieved by some module on a CAN bus.
void test_CAN_send( ) 
{
    int cantxValue = 60;

    Serial.print( "cantxValue: " );
    Serial.println( cantxValue );

    //Create data packet for CAN message
    unsigned char canMsg[ 8 ] = {   cantxValue, 
                                    0x00, 
                                    0x00, 
                                    0x00, 
                                    0x00, 
                                    0x00, 
                                    0x00, 
                                    0x00 };

    // send data:  id = 0x123, standrad frame, data len = 8, stmp: data buf
    CAN.sendMsgBuf( 0x07B, 0, 8, canMsg ); 
    delay( 250 );
}


// recieve a CAN frame sent from some module on a CAN bus.
void test_CAN_recieve( ) 
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

        Serial.print( "canRxValue:" );
        Serial.println( rx_frame.id );
        delay(250);
    }
}

//INSTALLATION TESTS

// turn steering wheel while the car is on and check that the readings from
// the torque sensors are as expected.      
void test_signal_sense( ) 
{
    int signal_a_read = 0;
    int signal_b_read = 0;
    float a_voltage_read = 0;
    float b_voltage_read = 0;

    signal_a_read = analogRead( SIGNAL_INPUT_A );
    signal_b_read = analogRead( SIGNAL_INPUT_B );

    a_voltage_read = ( signal_a_read * 5.0 ) / 1024.0;
    b_voltage_read = ( signal_b_read * 5.0 ) / 1024.0;

    Serial.print( "Signal A Value: " );
    Serial.print( a_voltage_read );

    Serial.print( "\tSignal B Value: " );
    Serial.println( b_voltage_read );
}


void test_torque_spoof( ) 
{
    // send spoofed torque, slowly sweep stering back and forth.
}


void test_hard_power_off( ) 
{
    // while sweeping the steering wheel, cut power and test that shutdown is
    // handled gracefully or throws the expected fault.
}


void setup( ) 
{
    init_serial();
    init_can();
}


void loop() 
{
    //test_DACS();
    //test_interrupt_relay();
    //test_CAN_send();
    //test_CAN_recieve();
    test_signal_sense();

}

