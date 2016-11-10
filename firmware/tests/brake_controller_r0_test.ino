// BENCH TESTS

test_power_drivers() {
  // in order to test that each PWM driving circuit function correctly, we can
  // drive LEDS, one by one, in a breathing patter (increasing and decreasing PWM).
}

test_pump_relay() {
  // to test that the pump relay functions corretly we can blink an LED using
  // the pump relay.
}

test_switch_relay() {
  // to test that the brake light switch spoofing relay functions we can blink two
  // LEDS in an alternating pattering using the the switch relay.
}

test_CAN_send() {
  // send a CAN frame, to be recieved by some module on a CAN bus.
}

test_can_recieve() {
  // recieve a CAN frame sent from some module on a CAN bus.

}

// INSTALLATION TESTS

test_pressure_response_matches_request() {
  // slowly build pressure at the brakes and check that the pressure at the brakes
  // matches the commanded pressure. This test confirms all solenoids including
  // master cylinder solenoids function as expected.
}

test_brake_pedal_input() {
  // put the system into a braking state, press the pedal. The module should
  // exit the braking state and allow the user to brake
}

test_brake_pedal_spoof() {
  // to test that the brake light switch spoofing relay functions we blink the
 // the brake lights.
}

setup() {

}

loop() {
  // interactively run tests

}
