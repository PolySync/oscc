#include "SPI.h"
#include "mcp_can.h"
#include "can_frame.h"
#include "common.h"



#define CAN_1_CS 9              // chip select pin for CAN1


#define CAN_2_CS 10             // chip select pin for CAN2


static MCP_CAN CAN_1( CAN_1_CS );


static MCP_CAN CAN_2( CAN_2_CS );


static void init_serial( void ) 
{
    Serial.begin( SERIAL_BAUD );
}


//
static void init_CAN_1( void )
{
    // wait until we have initialized
    // watchdog will reset if we take too long
    while( CAN_1.begin(CAN_BAUD) != CAN_OK )
    {
        // wait a little
        delay( CAN_INIT_RETRY_DELAY );
        Serial.println( "init_CAN_1: retrying" );
    }

    // debug log
    Serial.println( "init_CAN_1: pass" );
}


//
static void init_CAN_2( void )
{
    // wait until we have initialized
    // watchdog will reset if we take too long
    while( CAN_2.begin(CAN_BAUD) != CAN_OK )
    {
        // wait a little
        delay( CAN_INIT_RETRY_DELAY );
        Serial.println( "init_CAN_2: retrying" );

    }

    // debug log
    Serial.println( "init_CAN_2: pass" );
}


// recieve a CAN frame sent from some module on a CAN bus.
void test_CAN_1_recieve() {

    // local vars
    can_frame_s rx_frame;


    if( CAN_1.checkReceive() == CAN_MSGAVAIL )
    {
        memset( &rx_frame, 0, sizeof(rx_frame) );

        // read frame
        CAN_1.readMsgBufID(
                (INT32U*) &rx_frame.id,
                (INT8U*) &rx_frame.dlc,
                (INT8U*) rx_frame.data );
        Serial.println(rx_frame.id);
        Serial.println(rx_frame.data[0]);
        delay(250);
    }

}



// recieve a CAN frame sent from some module on a CAN bus.
void test_CAN_2_recieve() {

    // local vars
    can_frame_s rx_frame;

    Serial.flush(); 

    while( !Serial.available() )
    {

        if( CAN_2.checkReceive() == CAN_MSGAVAIL )
        {
            memset( &rx_frame, 0, sizeof(rx_frame) );

            // read frame
            CAN_2.readMsgBufID(
                    (INT32U*) &rx_frame.id,
                    (INT8U*) &rx_frame.dlc,
                    (INT8U*) rx_frame.data );
            Serial.println(rx_frame.id);
            Serial.println(rx_frame.data[0]);
            delay(250);
        }
    }

}


// send a CAN frame, to be recieved by some module on a CAN bus.
void test_CAN_1_send() {

    int cantxValue = 66;

    Serial.print("cantxValue: ");
    Serial.println(cantxValue);
    //Create data packet for CAN message
    unsigned char canMsg[8] = {cantxValue, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    // send data:  id = 0x123, standrad frame, data len = 8, stmp: data buf
    CAN_1.sendMsgBuf(0x07B, 0, 8, canMsg); 
    delay(250);
}


// send a CAN frame, to be recieved by some module on a CAN bus.
void test_CAN_2_send() {

    int cantxValue = 66;

    Serial.flush(); 

    while( !Serial.available() )
    {
        Serial.print("cantxValue: ");
        Serial.println(cantxValue);
        //Create data packet for CAN message
        unsigned char canMsg[8] = {cantxValue, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        // send data:  id = 0x123, standrad frame, data len = 8, stmp: data buf
        CAN_2.sendMsgBuf(0x07B, 0, 8, canMsg); 
        delay(250);
    }
}

void setup() {

    init_serial();
    init_CAN_1();
    //init_CAN_2();

}

void loop() {

    test_CAN_1_send();
    //test_CAN_2_send();
    test_CAN_1_recieve();
    //test_CAN_2_recieve();

}
