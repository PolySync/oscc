# language: en

Feature: Republishing OBD CAN frames

  Pertinent OBD CAN frames should be republished to the Control CAN bus.


  Scenario: Steering wheel angle OBD CAN frame received.
    When a steering wheel angle OBD CAN frame is received on the OBD CAN bus

    Then a steering wheel angle OBD CAN frame should be published to the Control CAN bus


  Scenario: Wheel speed OBD CAN frame received.
    When a wheel speed OBD CAN frame is received on the OBD CAN bus

    Then a wheel speed OBD CAN frame should be published to the Control CAN bus


  Scenario: Brake pressure OBD CAN frame received.
    When a brake pressure OBD CAN frame is received on the OBD CAN bus

    Then a brake pressure OBD CAN frame should be published to the Control CAN bus
