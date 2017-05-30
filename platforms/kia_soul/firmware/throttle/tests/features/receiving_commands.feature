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


  Scenario Outline: Accelerator position command sent from controller
    Given throttle control is enabled

    When the accelerator position command <command> is received

    Then the accelerator position command should be parsed
    And <spoof_val_high> should be written to DAC A
    And <spoof_val_low> should be written to DAC B

    Examples:
      | command  | spoof_val_high | spoof_val_low |
      |  0       |  599           |  299          |
      |  1000    |  626           |  313          |
      |  2000    |  654           |  327          |
      |  3000    |  681           |  340          |
      |  4000    |  708           |  354          |
      |  5000    |  735           |  367          |
      |  10000   |  872           |  436          |
      |  15000   |  1009          |  504          |
      |  19660   |  1136          |  568          |
