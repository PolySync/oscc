# language: en

Feature: Timeouts

  If the module doesn't receive an OBD report from the vehicle after an amount
  of time, warning flags should be set and valid flags cleared.


  Scenario: Steering wheel angle timeout
    When the time since the last received steering wheel angle report exceeds the timeout

    Then the steering wheel angle heartbeat warning should be set
    And the steering wheel angle valid flag should be cleared
    And the steering wheel angle rate valid flag should be cleared


  Scenario: Wheel speed timeout
    When the time since the last received wheel speed report exceeds the timeout

    Then the wheel speed heartbeat warning should be set
    And the wheel speed valid flag should be cleared


  Scenario: Brake pressure timeout
    When the time since the last received brake pressure report exceeds the timeout

    Then the brake pressure heartbeat warning should be set
    And the brake pressure valid flag should be cleared


  Scenario: Turn signal timeout
    When the time since the last received turn signal report exceeds the timeout

    Then the turn signal heartbeat warning should be set
    And the left turn signal flag should be cleared
    And the right turn signal flag should be cleared
    And the brake signal flag should be cleared
