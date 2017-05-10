# language: en

Feature: Receiving commands

  Commands received from a controller should be processed and acted upon.


  Scenario: Enable brake command sent from controller
    Given brake control is disabled

    When an enable brake command is received

    Then control should be enabled
    And the last command timestamp should be set


  Scenario: Disable brake command sent from controller
    Given brake control is enabled

    When a disable brake command is received

    Then control should be disabled
    And the last command timestamp should be set


  Scenario Outline: Brake pedal command sent from controller
    Given brake control is enabled
    And the current sensor reported brake pressure is <pressure>

    When the brake pedal command <command> is received

    Then the brake pedal command should be parsed

    Examples:
      | pressure | command |
      |  0       |  -4700  |
      |  25      |  -4000  |
      |  50      |  -3000  |
      |  100     |  -2000  |
      |  80      |  -1000  |
      |  60      |  0      |
      |  75      |  1000   |
      |  80      |  2000   |
