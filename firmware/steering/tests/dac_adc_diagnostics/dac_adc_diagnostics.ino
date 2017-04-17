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
    uint16_t dac_value;
    uint16_t dac_val_a;
    uint16_t dac_val_b;

    uint16_t spoof_a_adc_signal = 0;
    uint16_t spoof_b_adc_signal = 0;

    float spoof_a_adc_volts = 0.0;
    float spoof_b_adc_volts = 0.0;

    float dac_expected_output_a = 0.0;
    float dac_expected_output_b = 0.0;

    // energize the relay so we can read the values at the terminal
    digitalWrite( SPOOF_ENGAGE, HIGH );

    for ( dac_value = 0; dac_value < 4095; dac_value = dac_value + 15 )
    {
        dac_val_a = dac_value;
        dac_val_b = 4095.0 - dac_value;

        // Convert 12-bit DAC output integer value to volts ( 5V / 4096steps )
        // Maximum voltage is 5 Volts.
        dac_expected_output_a = ( 5.0 / 4095.0 ) * dac_val_a;
        dac_expected_output_b = ( 5.0 / 4095.0 ) * dac_val_b;

        dac.outputA( dac_val_a );
        dac.outputB( dac_val_b );

        delay( 2000 );

        spoof_a_adc_signal = analogRead( SPOOF_SIGNAL_A );
        spoof_b_adc_signal = analogRead( SPOOF_SIGNAL_B );

        // Convert 10-bit ADC input integer value to volts ( 5V / 1024steps )
        // Maximum voltage is 5 Volts.
        spoof_a_adc_volts = ( spoof_a_adc_signal * 5.0 ) / 1023.0;
        spoof_b_adc_volts = ( spoof_b_adc_signal * 5.0 ) / 1023.0;

        Serial.print( "DAC Value (A): " );
        Serial.print( dac_val_a );

        Serial.print( "\tDAC Value (B): " );
        Serial.print( dac_val_b );

        Serial.print( "\tOutput A Voltage: " );
        Serial.print( dac_expected_output_a, 3 );

        Serial.print( "\tOutput B Voltage: " );
        Serial.println( dac_expected_output_b, 3 );

        Serial.print( "Spoof A Value: " );
        Serial.print( spoof_a_adc_signal );

        Serial.print( "\tSpoof B Value: " );
        Serial.print( spoof_b_adc_signal );

        Serial.print( "\tSpoof A Voltage: " );
        Serial.print( spoof_a_adc_volts, 3 );

        Serial.print( "\tSpoof B Voltage: " );
        Serial.println( spoof_b_adc_volts, 3 );

        Serial.println( "" );
    }
}


void setup( )
{
    init_serial();
    init_can();
}


void loop()
{
    test_DACS();
    //test_interrupt_relay();
    //test_CAN_send();
    //test_CAN_recieve();
    //test_signal_sense();

}
