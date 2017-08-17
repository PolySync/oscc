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

    When a command is received with spoof values <high> and <low>

    Then <high> should be sent to DAC A
    And <low> should be sent to DAC B

    Examples:
      | high  | low    |
      |  3440 |  656  |
      |  2500 |  1500  |
      |  2000 |  2000  |
      |  1500 |  2500  |
      |  738  |  3358  |


  Scenario Outline: Spoof value sent from application outside valid range
    Given steering control is enabled

    When a command is received with spoof values <high> and <low>

    Then <high_clamped> should be sent to DAC A
    And <low_clamped> should be sent to DAC B

    Examples:
      | high  | low   | high_clamped | low_clamped |
      |  4000 |  0    | 3440         |  656        |
      |  3500 |  500  | 3440         |  656        |
      |  500  |  3500 | 738          |  3358       |
      |  0    |  4000 | 738          |  3358       |
