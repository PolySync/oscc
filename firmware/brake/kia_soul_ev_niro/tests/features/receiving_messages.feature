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

    When a command is received with spoof values <high> and <low>

    Then <high> should be sent to DAC A
    And <low> should be sent to DAC B

    Examples:
      | high   | low    |
      |  1876  |  273  |
      |  1800  |  300  |
      |  1500  |  500  |
      |  1000  |  750  |
      |  750   |  900  |
      |  572   |  917  |


  Scenario Outline: Spoof value sent from application outside valid range
    Given brake control is enabled

    When a command is received with spoof values <high> and <low>

    Then <high_clamped> should be sent to DAC A
    And <low_clamped> should be sent to DAC B

    Examples:
      | high  | low   | high_clamped | low_clamped |
      |  4000 |  0    | 1876         |  273        |
      |  3500 |  500  | 1876         |  500        |
      |  500  |  3500 | 572          |  917        |
      |  0    |  4000 | 572          |  917        |
