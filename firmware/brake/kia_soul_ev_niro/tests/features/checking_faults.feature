# language: en

Feature: Checking for faults

  If the module encounters a fault condition, it should disable control and
  publish a fault report.


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
      |  250       |
      |  500       |
      |  1000      |
