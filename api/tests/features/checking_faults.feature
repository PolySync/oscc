# language: en

Feature: Checking for faults

  If the module encounters a fault condition, it should disable control and
  publish a fault report.

  Scenario: A sensor becomes temporarily disconnected
    Given throttle control is enabled

    When the operator applies to the accelerator

    Then control should be enabled
