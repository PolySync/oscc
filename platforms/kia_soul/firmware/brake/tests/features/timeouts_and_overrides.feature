# language: en

Feature: Timeouts and overrides

  If the module doesn't hear from the controller after an amount of time,
  or the operator manually actuates the brake pedal, control should be
  disabled.


  Scenario: Controller command timeout
    Given brake control is enabled

    When the time since the last received controller command exceeds the timeout

    Then control should be disabled


  Scenario Outline: Operator override
    Given brake control is enabled

    When the operator applies <sensor_val> to the brake pedal

    Then control should be disabled
    And override flag should be set

    Examples:
      | sensor_val |
      |  256       |
      |  512       |
      |  1024      |
      |  2048      |
      |  4096      |
