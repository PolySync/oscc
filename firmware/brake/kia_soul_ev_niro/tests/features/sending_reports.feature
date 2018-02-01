# language: en

Feature: Sending reports

  Brake reports should be published to the control CAN bus.


  Scenario: Brake report published
    When a brake report is published

    Then a brake report should be put on the control CAN bus
    And the brake report's fields should be set
