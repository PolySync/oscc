# language: en

Feature: Receiving commands

  Commands received from a controller should be processed and acted upon.

  @enable_command
  Scenario: Enable brake command sent from controller
    Given brake control is disabled

    When an enable brake command is received

    Then control should be enabled
    And the last command timestamp should be set

  @disable_command
  Scenario: Disable brake command sent from controller
    Given brake control is enabled

    When a disable brake command is received

    Then control should be disabled
    And the last command timestamp should be set


  Scenario Outline: Brake pedal command sent from controller
    Given brake control is enabled
    And the left brake sensor reads <left_pressure>
    And the right brake sensor reads <right_pressure>

    When the brake pedal command <command> is received

    Then the brake pedal command should be parsed
    And the <solenoid> solenoid should be activated with duty cycle <duty_cycle>

    Examples:
      | left_pressure | right_pressure | command | solenoid     | duty_cycle |
      |  120          |  120           |  20000  |  ACCUMULATOR |  105       |
      |  160          |  160           |  20000  |  ACCUMULATOR |  100       |
      |  190          |  190           |  20000  |  ACCUMULATOR |  89        |
      |  230          |  230           |  20000  |  RELEASE     |  72        |
      |  200          |  200           |  20000  |  ACCUMULATOR |  88        |
      |  220          |  220           |  20000  |  NONE        |  0         |
      |  205          |  205           |  20000  |  ACCUMULATOR |  86        |
      |  215          |  215           |  20000  |  NONE        |  0         |
