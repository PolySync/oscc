# language: en

Feature: Sending reports

  Throttle reports should be published to the control CAN bus after an interval.


  Scenario Outline: Throttle report published after an interval
    Given the accelerator position sensors have a reading of <sensor_val>
    And the previous accelerator position command was <command>

    When the time since the last report publishing exceeds the interval

    Then a throttle report should be published to the control CAN bus
    And the report's command field should be set to <command>
    And the report's current_accelerator_position field should be set to <current_accelerator_position>

    Examples:
      | command | sensor_val | current_accelerator_position |
      |  0      |  0         |  0                           |
      |  50     |  256       |  512                         |
      |  100    |  512       |  1408                        |
