# language: en

Feature: Receiving commands

  Commands received from a application should be processed and acted upon.


  Scenario: Enable brake command sent from application
    Given brake control is disabled

    When an enable brake command is received

    Then control should be enabled


  Scenario: Disable brake command sent from application
    Given brake control is enabled

    When a disable brake command is received

    Then control should be disabled


  Scenario: Fault report sent from a different module
    Given brake control is enabled

    When a fault report is received

    Then control should be disabled


  Scenario Outline: Spoof value sent from application
    Given brake control is enabled

    When a command is received with request value <value>

    Then <high> should be sent to DAC A
    And <low> should be sent to DAC B

    Examples:
      | value | high   | low  |
      | 1     | 1875   | 917  |
      | 0.942 | 1800   | 880  |
      | 0.712 | 1500   | 731  |
      | 0.329 | 1000   | 484  |
      | 0.137 | 750    | 361  |
      | 0     | 572    | 273  |


  Scenario Outline: Spoof value sent from application outside valid range
    Given brake control is enabled

    When a command is received with request value <value>

    Then <high_clamped> should be sent to DAC A
    And <low_clamped> should be sent to DAC B

    Examples:
      | value | high_clamped | low_clamped |
      | 5     | 1875         |  917        |
      | 2     | 1875         |  917        |
      | -1    | 572          |  273        |
      | -2    | 572          |  273        |
