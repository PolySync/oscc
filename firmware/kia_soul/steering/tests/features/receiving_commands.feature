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


  Scenario Outline: Spoof value sent from application
    Given steering control is enabled

    When a command is received with spoof values <high> and <low>

    Then <high> should be sent to DAC A
    And <low> should be sent to DAC B

    Examples:
      | high  | low   |
      |  1064 |  2834 |
      |  3031 |  868  |
      |  1064 |  2834 |
      |  1064 |  2834 |
      |  1707 |  2191 |
      |  2889 |  1009 |
      |  3031 |  868  |
      |  3031 |  868  |
      |  3031 |  868  |
      |  3031 |  868  |
      |  3031 |  868  |
