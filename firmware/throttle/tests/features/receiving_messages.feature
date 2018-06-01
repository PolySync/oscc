# language: en

Feature: Receiving commands

  Commands received from a application should be processed and acted upon.


  Scenario: Enable throttle command sent from application
    Given throttle control is disabled

    When an enable throttle command is received

    Then control should be enabled


  Scenario: Disable throttle command sent from application
    Given throttle control is enabled

    When a disable throttle command is received

    Then control should be disabled


  Scenario: Fault report sent from a different module
    Given throttle control is enabled

    When a fault report is received

    Then control should be disabled


  Scenario Outline: Spoof value sent from application
    Given throttle control is enabled

    When a command is received with request value <value>

    Then <high> should be sent to DAC A
    And <low> should be sent to DAC B

    Examples:
      | value | high                                 | low                                 |
      | 1     | THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MAX | THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MAX |
      | 0     | THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MIN | THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MIN |


  Scenario Outline: Spoof value sent from application outside valid range
    Given throttle control is enabled

    When a command is received with request value <value>

    Then <high_clamped> should be sent to DAC A
    And <low_clamped> should be sent to DAC B

    Examples:
      | value | high_clamped                         | low_clamped                         |
      | 5     | THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MAX | THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MAX |
      | 1.1   | THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MAX | THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MAX |
      | -0.1  | THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MIN | THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MIN |
      | -5    | THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MIN | THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MIN |
