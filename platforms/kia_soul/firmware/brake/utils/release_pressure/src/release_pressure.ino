// Brake pressure release ECU firmware
// 2004-2009 Prius brake actuator


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
const float ZERO_PRESSURE = 0.48;        // The voltage the sensors read when no pressure is present
const float MIN_PACC = 2.3;              // minumum accumulator pressure to maintain
const float MAX_PACC = 2.4;              // max accumulator pressure to maintain
const float PEDAL_THRESH = 0.6;          // Pressure for pedal interference


int deltaT=10,
    currMicros,
    lastMicros = 0;

unsigned long previousMillis=0;

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


// master Solenoid structure
struct SMC {
    float _pressure1 = 0.0; // Initialize pressures to 0.0 to avoid false values
    float _pressure2 = 0.0;
    byte _sensor1Pin = 99;
    byte _sensor2Pin = 99;
    byte _controlPin = 99;

    SMC( byte sensor1Pin, byte sensor2Pin, byte controlPin );

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
SMC smc(PIN_PMC1, PIN_PMC2, PIN_SMC);
Brakes brakes = Brakes( PIN_PFL, PIN_PFR, PIN_SLAFL, PIN_SLAFR, PIN_SLRFL, PIN_SLRFR);


// the setup routine runs once when you press reset:
void setup( void )
{
    // set the Arduino's PWM timers to 3.921 KHz, above the acoustic range
    TCCR3B = (TCCR3B & 0xF8) | 0x02; // pins 2,3,5 | timer 3
    TCCR4B = (TCCR4B & 0xF8) | 0x02; // pins 6,7,8 | timer 4

    digitalWrite( PIN_BRAKE_SWITCH, LOW );
    pinMode( PIN_BRAKE_SWITCH, OUTPUT );

    // depower all the things
    digitalWrite( PIN_PUMP, LOW );
    smc.solenoidsOpen();

    // close rear slrs. These should open only for emergencies and to release brake pressure
    brakes.depowerSLR();

    // Clear any pressure in the accumulator
    brakes.powerSLA(250);
    delay(20000);

    // initialize for braking
    brakes.depowerSLA();

    Serial.begin( 115200 );
}

void loop()
{


    float accumulator_sensor = convertToVoltage(analogRead(9));
    Serial.print("pressure: ");
    Serial.println(accumulator_sensor);

}
