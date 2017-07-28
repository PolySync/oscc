# language: en

Feature: Republishing OBD CAN frames

  All OBD CAN frames should be republished to the Control CAN bus.


  Scenario: OBD CAN frame received.
    When an OBD CAN frame is received on the OBD CAN bus

    Then an OBD CAN frame should be published to the Control CAN bus
