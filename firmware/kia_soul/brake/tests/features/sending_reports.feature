# language: en

Feature: Sending reports

  Brake reports should be published to the control CAN bus after an interval.


  Scenario Outline: Brake report published after an interval
    Given the previous brake pedal position command was <command>
    And the current vehicle reported brake pressure is <vehicle_brake_pressure>
    And the current sensor reported brake pressure is <sensor_brake_pressure>

    When the time since the last report publishing exceeds the interval

    Then a brake report should be published to the control CAN bus
    And the report's command field should be set to <command>
    And the report's current vehicle reported brake pressure field should be set to <vehicle_brake_pressure>
    And the report's sensor reported brake pressure should be set to <sensor_brake_pressure>

    Examples:
      | command | vehicle_brake_pressure | sensor_brake_pressure |
      |  0      |  -512                  |  256                  |
      |  50     |  256                   |  512                  |
      |  100    |  512                   |  1024                 |
