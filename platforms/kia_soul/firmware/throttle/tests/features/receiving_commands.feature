# language: en

Feature: Receiving commands

  Commands received from a controller should be processed and acted upon.


  Scenario Outline: Enable throttle command sent from controller
    Given throttle control is disabled
    And the accelerator position sensors have a reading of <sensor_val>

    When an enable throttle command is received

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


  Scenario Outline: Disable throttle command sent from controller
    Given throttle control is enabled
    And the accelerator position sensors have a reading of <sensor_val>

    When a disable throttle command is received

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


  Scenario Outline: Accelerator target command sent from controller
    Given throttle control is enabled

    When the accelerator target command <command> is received

    Then the accelerator target command should be parsed
    And <spoof_val_high> should be written to DAC A
    And <spoof_val_low> should be written to DAC B

    Examples:
      | command  | spoof_val_high | spoof_val_low |
      |  0       |  599           |  299          |
      |  1000    |  1255          |  627          |
      |  2000    |  1910          |  955          |
      |  3000    |  2565          |  1282         |
      |  4000    |  3221          |  1610         |
      |  5000    |  3500          |  1800         |
      |  10000   |  3500          |  1800         |
      |  15000   |  3500          |  1800         |
      |  19660   |  3500          |  1800         |
