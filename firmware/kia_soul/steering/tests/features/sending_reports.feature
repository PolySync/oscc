# language: en

Feature: Sending reports

  Steering reports should be published to the control CAN bus.


  Scenario: Steering report published
    When a steering report is published

    Then a steering report should be put on the control CAN bus
    And the steering report's enabled field should be set
    And the steering report's override field should be set
    And the steering report's DTCs field should be set


  Scenario: Fault report published
    When a fault report is published

    Then a fault report should be put on the control CAN bus
    And the fault report's origin ID field should be set
