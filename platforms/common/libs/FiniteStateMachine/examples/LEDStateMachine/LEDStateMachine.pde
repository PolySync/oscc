/*
||
|| @file 	GIGAMAXX_TLC5940.pde
|| @version	1.0
|| @author	Alexander Brevig
|| @contact	alexanderbrevig@gmail.com
||
|| @description
|| | Interface with the TLC5940
|| | Use a button to control the state and animation of the LEDs
|| | Current states:
|| |  * no operation
|| |  * slow fade
|| |  * repetative flash
|| |    * 'circular fade' animation
|| |
|| | As the program is now, you can press the button again to start over at 'no operation'
|| #
||
*/
/**
	README
	This code might seem over compicated and over engineered but it is highly maintainable.
	The code uses the design pattern of finite state machines, 
	also it incorporates hardware abstraction of both the TLC and the Button used.
	
	I have not tested this code on hardware, simply because I have no tlc5940 around
	This means that I do not know if the program works as intended [like the curvature of the fade, or the fade at all]
	I am sorry about that, but if you have to run the code by me more that two times I'll do it for free, full 'refund'
	If we do more project I'll be sure to get my hands on the actual hardware
	
	Adding additional features or steps/states is simple
	 1) change the NUMBER_OF_SELECATBLE_STATES constant if you add a new selective state such as fade and blink/animate
	 2) add a definition of the state
	 3) add a new case to the CONTROL THE STATE section at line 89
	 4) implement the Update function of the state [like fadeUpdate, circleMotionUpdate etc]
	 5) upload new code to the Arduino and enjoy
*/
//http://www.arduino.cc/playground/uploads/Code/FSM_1-6.zip
#include <FiniteStateMachine.h>
//http://www.arduino.cc/playground/uploads/Code/Button.zip
#include <Button.h>
//http://code.google.com/p/tlc5940arduino/
#include "Tlc5940.h"
#include "tlc_fades.h"

/** program config and constants */
//WIRE THIS DIRECTLY FROM GROUND TO PIN
const byte BUTTON_PIN = 2; 
//how many states are we cycling through?
const byte NUMBER_OF_SELECATBLE_STATES = 3; 
//modify this if you want the fade and the flash to be of lesser intensity
const int  FULL_INTENSITY = 4095; 

/** circle config */
const uint16_t CIRCLE_DURATION = 200;
const int      CIRCLE_MAX      = FULL_INTENSITY / 2;

/** fade config */
const uint16_t FADE_DURATION = 10 * 1000; //the fade should last 10 seconds

/** flash config */
const int FLASH_ITERATIONS = 6;
const int FLASH_INTERVAL = 50;

/** this is the definitions of the states that our program uses */
State noop = State(noopUpdate);  //no operation
State fade = State(fadeEnter, fadeUpdate, NULL);  //this state fades the LEDs in
State flash = State(flashUpdate);  //this state flashes the leds FLASH_ITERATIONS times at 1000/FLASH_INTERVAL
State circleMotion = State(circleMotionUpdate); //show the circular animation

/** the state machine controls which of the states get attention and execution time */
FSM stateMachine = FSM(noop); //initialize state machine, start in state: noop

/** this is the control interface, and is used to change the state of the program */
Button button = Button(BUTTON_PIN,PULLUP); //initialize the button (wire between BUTTON_PIN and ground)

void setup(){ 
  //initialize the Tlc library
  Tlc.init();
  //do we need to explicitly tell the tlc that all leds start off?
}

//poor example, but then again; it's just an example
void loop(){
  //counter variable, holds number of button presses
  static byte buttonPresses = 0; //only accessible from this function, value is kept between iterations
  
  if (button.uniquePress()){
    //increment buttonPresses and constrain it to [0, NUMBER_OF_SELECATBLE_STATES-1]
    buttonPresses = ++buttonPresses % NUMBER_OF_SELECATBLE_STATES; 
	/*
	  manipulate the state machine by external input and control
	*/
	//CONTROL THE STATE
    switch (buttonPresses){
      case 0: stateMachine.transitionTo(noop); break;
      case 1: stateMachine.transitionTo(fade); break; //first press
      case 2: stateMachine.transitionTo(flash); break; //second press
    }
  }
  //THIS LINE IS CRITICAL
  //do not remove the stateMachine.update() call, it is what makes this program 'tick'
  stateMachine.update();
}

/*
  ALL the functions below are helper functions for the states of the program
*/
///[noop state:update] the state machine is in a state that does nothing
void noopUpdate() {
  //this function gets called as long as the user have not pressed any buttons after startup
}
///[fade state:enter] the statemachine is just told to enter the fade state, we need to 
void fadeEnter() {
  uint32_t startMillis = millis() + 50;
  uint32_t endMillis = startMillis + FADE_DURATION;
  for (int i=0; i<NUM_TLCS * 16;i++) {
    tlc_addFade(i, 0, FULL_INTENSITY, startMillis, endMillis);
    //tlc_addFade(channel, FULL_INTENSITY, 0, endMillis, endMillis + FADE_DURATION); fade back down?
  }
}
///[fade state:update] we need to keep updating the tlc in order to see the effects
void fadeUpdate() {
  tlc_updateFades();
}
///[flash state:update] this state blocks loop until done and does an immediateTransition to circle state
void flashUpdate() {
  for (int times=0; times<FLASH_ITERATIONS; times++) {
    setAll(LOW);
	delay(FLASH_INTERVAL/2);
    setAll(HIGH);
	delay(FLASH_INTERVAL/2);
  }
  stateMachine.immediateTransitionTo(circleMotion);
}
///helper for flash state, set all LEDs either fully off or fully on
void setAll(boolean state) {
  Tlc.clear();
  for (TLC_CHANNEL_TYPE channel = 0; channel < NUM_TLCS * 16; channel += direction) {
    Tlc.set(channel, (state?FULL_INTENSITY:0) );
  }
  Tlc.update();
}
///[circle state:update] animate the circle pattern
void circleMotionUpdate() {
  static TLC_CHANNEL_TYPE channel = 0; //only this function can access the channel variable, and the value is kept between calls
  if (tlc_fadeBufferSize < TLC_FADE_BUFFER_LENGTH - 2) {
    if (!tlc_isFading(channel)) {
      uint16_t duration = CIRCLE_DURATION;
      int maxValue = CIRCLE_MAX;
      uint32_t startMillis = millis() + 50;
      uint32_t endMillis = startMillis + duration;
      tlc_addFade(channel, 0, maxValue, startMillis, endMillis);
      tlc_addFade(channel, maxValue, 0, endMillis, endMillis + duration);
    }
    if (channel++ == NUM_TLCS * 16) {
      channel = 0;
    }
  }
  tlc_updateFades();
}