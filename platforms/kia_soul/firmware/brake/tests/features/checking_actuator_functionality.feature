# language: en

Feature: Checking actuator functionality

  Brake actuator should be checked for proper functionality on module startup.


  Scenario: Startup checks fail
    Given the actuator and pump motor are in a bad state

    When the startup checks are run

    Then the actuator error should be set to true
    And the pump motor error should be set to true


  Scenario: Startup checks pass
    Given the actuator and pump motor are in a good state

    When the startup checks are run

    Then the actuator error should be set to false
    And the pump motor error should be set to false
