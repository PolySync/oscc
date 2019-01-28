# language: en

Feature: Receiving commands

  Commands received from a application should be processed and acted upon.


  Scenario: Enable steering command sent from application
    Given steering control is disabled

    When an enable steering command is received

    Then control should be enabled


  Scenario: Disable steering command sent from application
    Given steering control is enabled

    When a disable steering command is received

    Then control should be disabled


  Scenario: Fault report sent from a different module
    Given steering control is enabled

    When a fault report is received

    Then control should be disabled


  Scenario Outline: Spoof value sent from application
    Given steering control is enabled

    When a command is received with request value <value>

    Then <high> should be sent to DAC A
    And <low> should be sent to DAC B

    Examples:
      | value | high                                 | low                                 |
      | -1    | STEERING_SPOOF_HIGH_SIGNAL_RANGE_MAX | STEERING_SPOOF_LOW_SIGNAL_RANGE_MIN |
      | 1     | STEERING_SPOOF_HIGH_SIGNAL_RANGE_MIN | STEERING_SPOOF_LOW_SIGNAL_RANGE_MAX |


  Scenario Outline: Spoof value sent from application outside valid range
    Given steering control is enabled

    When a command is received with request value <value>

    Then <high_clamped> should be sent to DAC A
    And <low_clamped> should be sent to DAC B

    Examples:
      | value | high_clamped                         | low_clamped                         |
      | -15   | STEERING_SPOOF_HIGH_SIGNAL_RANGE_MAX | STEERING_SPOOF_LOW_SIGNAL_RANGE_MIN |
      | -1.1  | STEERING_SPOOF_HIGH_SIGNAL_RANGE_MAX | STEERING_SPOOF_LOW_SIGNAL_RANGE_MIN |
      | 1.1   | STEERING_SPOOF_HIGH_SIGNAL_RANGE_MIN | STEERING_SPOOF_LOW_SIGNAL_RANGE_MAX |
      | 15    | STEERING_SPOOF_HIGH_SIGNAL_RANGE_MIN | STEERING_SPOOF_LOW_SIGNAL_RANGE_MAX |
