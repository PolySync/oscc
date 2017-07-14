# language: en

Feature: Checking sensor validity

  Invalid values read from sensors should cause control to be disabled.


  Scenario: A sensor becomes temporarily disconnected
    Given throttle control is enabled

    When a sensor becomes temporarily disconnected

    Then control should remain enabled


  Scenario: A sensor becomes permanently disconnected
    Given throttle control is enabled

    When a sensor becomes permanently disconnected

    Then control should be disabled
