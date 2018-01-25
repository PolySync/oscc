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
      | value |  high  |  low   |
      | 1     |  3358  |  1638  |
      | 0.8   |  2801  |  1359  |
      | 0.6   |  2244  |  1081  |
      | 0.5   |  1966  |  942   |
      | 0.4   |  1687  |  802   |
      | 0.2   |  1130  |  524   |
      | 0     |  573   |  245   |


  Scenario Outline: Spoof value sent from application outside valid range
    Given throttle control is enabled

    When a command is received with request value <value>

    Then <high_clamped> should be sent to DAC A
    And <low_clamped> should be sent to DAC B

    Examples:
      | value | high_clamped | low_clamped |
      |  5    | 3358         |  1638       |
      |  1.1  | 3358         |  1638       |
      |  -0.1 | 573          |  245        |
      |  -5   | 573          |  245        |
