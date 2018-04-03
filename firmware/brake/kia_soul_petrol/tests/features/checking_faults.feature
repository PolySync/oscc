# language: en

Feature: Timeouts and overrides

  If the module doesn't hear from the controller after an amount of time,
  or the operator manually actuates the brake pedal, control should be
  disabled.


  Scenario: A sensor becomes temporarily disconnected
    Given brake control is enabled

    When a sensor is grounded for 50 ms

    Then control should remain enabled


  Scenario: A sensor becomes permanently disconnected
    Given brake control is enabled

    When a sensor is grounded for 200 ms

    Then control should be disabled
    And a fault report should be published


  Scenario Outline: Operator override
    Given brake control is enabled

    When the operator applies <sensor_val> to the brake pedal for 200 ms

    Then control should be disabled
    And a fault report should be published

    Examples:
      | sensor_val |
      |  256       |
      |  512       |
      |  1024      |
      |  2048      |
      |  4096      |
