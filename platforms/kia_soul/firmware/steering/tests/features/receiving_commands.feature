# language: en

Feature: Receiving commands

  Commands received from a controller should be processed and acted upon.


  Scenario Outline: Enable steering command sent from controller
    Given steering control is disabled
    And the torque sensors have a reading of <sensor_val>

    When an enable steering command is received

    Then control should be enabled
    And the last command timestamp should be set
    And <dac_a_val> should be written to DAC A
    And <dac_b_val> should be written to DAC B

    Examples:
      | sensor_val | dac_a_val   | dac_b_val   |
      |  0         |  0          |  0          |
      |  256       |  1024       |  1024       |
      |  512       |  2048       |  2048       |
      |  1024      |  4096       |  4096       |


  Scenario Outline: Disable steering command sent from controller
    Given steering control is enabled
    And the torque sensors have a reading of <sensor_val>

    When a disable steering command is received

    Then control should be disabled
    And the last command timestamp should be set
    And <dac_a_val> should be written to DAC A
    And <dac_b_val> should be written to DAC B

    Examples:
      | sensor_val | dac_a_val   | dac_b_val   |
      |  0         |  0          |  0          |
      |  256       |  1024       |  1024       |
      |  512       |  2048       |  2048       |
      |  1024      |  4096       |  4096       |


  Scenario Outline: Steering wheel angle command sent from controller
    Given steering control is enabled
    And the current steering wheel angle is <angle>

    When the steering wheel angle command <command> with angle rate 111 is received

    Then the steering wheel angle command should be parsed
    And <spoof_val_low> should be written to DAC A
    And <spoof_val_high> should be written to DAC B

    Examples:
      | angle  | command | spoof_val_high | spoof_val_low |
      |  -2048 |  -4700  |  1064          |  2834         |
      |  -1024 |  -4000  |  3031          |  868          |
      |  -512  |  -3000  |  1064          |  2834         |
      |  -256  |  -2000  |  1064          |  2834         |
      |  -128  |  -1000  |  1707          |  2192         |
      |  0     |  0      |  2889          |  1009         |
      |  128   |  1000   |  3031          |  868          |
      |  256   |  2000   |  3031          |  868          |
      |  512   |  3000   |  3031          |  868          |
      |  1024  |  4000   |  3031          |  868          |
      |  2048  |  4700   |  3031          |  868          |
