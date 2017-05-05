# language: en

Feature: Sending reports

  Throttle reports should be published to the control CAN bus after an interval.


  Scenario Outline: Throttle report published after an interval
    Given the accelerator position sensors have a reading of <sensor_val>
    And the previous accelerator command was <command>

    When the time since the last report publishing exceeds the interval

    Then a throttle report should be published to the control CAN bus
    And the report's command field should be set to <command>
    And the report's accelerator_input field should be set to <accelerator_input>

    Examples:
      | command | sensor_val | accelerator_input |
      |  0      |  0         |  0                |
      |  50     |  256       |  2048             |
      |  100    |  512       |  4096             |
