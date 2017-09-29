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
      |  2300  |  250  |
      |  2000  |  300  |
      |  1500  |  500  |
      |  1000  |  750  |
      |  750   |  1000  |
      |  550   |  1100  |


  Scenario Outline: Spoof value sent from application outside valid range
    Given brake control is enabled

    When a command is received with spoof values <high> and <low>

    Then <high_clamped> should be sent to DAC A
    And <low_clamped> should be sent to DAC B

    Examples:
      | high  | low   | high_clamped | low_clamped |
      |  4000 |  0    | 2351         |  248        |
      |  3500 |  500  | 2351         |  500        |
      |  500  |  3500 | 520          |  1146       |
      |  0    |  4000 | 520          |  1146       |
