# language: en

Feature: Checking general API functionality

  If the API recieves commands to open or close the CAN connection, or enable or disable the firmware modules, it should respond correctly and not return any errors.

  Scenario: OSCC enable
    Given the oscc connection has been opened

    When an enable command is recieved

    Then no error should be reported
    And the modules should be enabled

  Scenario: OSCC disable
    Given the oscc connection has been opened 
    And the oscc modules have been enabled

    When a disable command is recieved

    Then no error should be reported
    And the modules should be disabled

  Scenario: OSCC steering
    Given the oscc connection has been opened 
    And the oscc modules have been enabled

    When a steering command is recieved

    Then no error should be reported
    And the steering module should still be enabled