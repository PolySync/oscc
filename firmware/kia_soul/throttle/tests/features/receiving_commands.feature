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


  Scenario Outline: Spoof value sent from application
    Given throttle control is enabled

    When a command is received with spoof values <high> and <low>

    Then <high> should be sent to DAC A
    And <low> should be sent to DAC B

    Examples:
      | high  | low  |
      |  599  |  299 |
      |  626  |  313 |
      |  654  |  327 |
      |  681  |  340 |
      |  708  |  354 |
      |  735  |  367 |
      |  872  |  436 |
      |  1009 |  504 |
      |  1136 |  568 |
