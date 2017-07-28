# language: en

Feature: Sending reports

  Steering reports should be published to the control CAN bus.


  Scenario: Steering report published
    When a steering report is published

    Then a steering report should be put on the control CAN bus
    And the steering report's fields should be set
