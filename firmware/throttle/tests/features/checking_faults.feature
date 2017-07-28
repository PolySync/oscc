# language: en

Feature: Checking for faults

  If the module encounters a fault condition, it should disable control and
  publish a fault report.

  Scenario: A sensor becomes temporarily disconnected
    Given throttle control is enabled

    When a sensor becomes temporarily disconnected

    Then control should remain enabled


  Scenario: A sensor becomes permanently disconnected
    Given throttle control is enabled

    When a sensor becomes permanently disconnected

    Then control should be disabled
    And a fault report should be published


  Scenario: Controller command timeout
    Given throttle control is enabled

    When the time since the last received controller command exceeds the timeout

    Then control should be disabled
    And a fault report should be published


  Scenario Outline: Operator override
    Given throttle control is enabled

    When the operator applies <sensor_val> to the accelerator

    Then control should be disabled
    And a fault report should be published

    Examples:
      | sensor_val |
      |  250       |
      |  500       |
      |  1000      |
