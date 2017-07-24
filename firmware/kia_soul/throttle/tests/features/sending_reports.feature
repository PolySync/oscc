# language: en

Feature: Sending reports

  Throttle reports should be published to the control CAN bus.


  Scenario: Throttle report published
    When a throttle report is published

    Then a throttle report should be put on the control CAN bus
    And the throttle report's enabled field should be set
    And the throttle report's override field should be set
    And the throttle report's DTCs field should be set
