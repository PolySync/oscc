# language: en

Feature: Receiving commands

  Commands received from a controller should be processed and acted upon.


  Scenario: Enable brake command sent from controller
    Given brake control is disabled

    When an enable brake command is received

    Then control should be enabled


  Scenario: Disable brake command sent from controller
    Given brake control is enabled

    When a disable brake command is received

    Then control should be disabled


  Scenario: Fault report sent from a different module
    Given brake control is enabled

    When a fault report is received

    Then control should be disabled
