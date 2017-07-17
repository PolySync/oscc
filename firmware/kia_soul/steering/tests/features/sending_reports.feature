# language: en

Feature: Sending reports

  Steering reports should be published to the control CAN bus after an interval.


  Scenario Outline: Steering report published after an interval
    Given the torque sensors have a reading of <sensor_val>
    And the previous steering wheel angle command was <command>
    And the current steering wheel angle is <angle>
    And the spoofed torque output was <torque>

    When the time since the last report publishing exceeds the interval

    Then a steering report should be published to the control CAN bus
    And the report's command field should be set to <command>
    And the report's steering wheel angle field should be set to <angle>
    And the report's torque output field should be set to <torque>

    Examples:
      | command | sensor_val | angle  | torque |
      |  0      |  0         |  -2925 |  32    |
      |  50     |  256       |  45    |  64    |
      |  100    |  512       |  731   |  127   |
