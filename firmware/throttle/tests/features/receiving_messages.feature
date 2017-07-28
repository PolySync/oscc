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

    When a command is received with spoof values <high> and <low>

    Then <high> should be sent to DAC A
    And <low> should be sent to DAC B

    Examples:
      | high   | low    |
      |  3500  |  0    |
      |  3000  |  500  |
      |  2500  |  1000 |
      |  2000  |  1500 |
      |  1500  |  1800 |
      |  1000  |  1800 |
      |  500   |  1800 |
      |  0     |  1800 |


  Scenario Outline: Spoof value sent from application outside valid range
    Given throttle control is enabled

    When a command is received with spoof values <high> and <low>

    Then <high_clamped> should be sent to DAC A
    And <low_clamped> should be sent to DAC B

    Examples:
      | high  | low   | high_clamped | low_clamped |
      |  4000 |  0    | 3500         |  0          |
      |  3500 |  500  | 3500         |  500        |
      |  500  |  3500 | 500          |  1800       |
      |  0    |  4000 | 0            |  1800       |
