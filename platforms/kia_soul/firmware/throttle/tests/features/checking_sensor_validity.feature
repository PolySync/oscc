# language: en

Feature: Checking sensor validity

  Invalid values read from sensors should cause control to be disabled.


  Scenario: Accelerator position sensor becomes temporarily disconnected
    Given throttle control is enabled

    When the accelerator position sensor becomes temporarily disconnected

    Then control should remain enabled


  Scenario: Accelerator position sensor becomes permanently disconnected
    Given throttle control is enabled

    When the accelerator position sensor becomes permanently disconnected

    Then control should be disabled
