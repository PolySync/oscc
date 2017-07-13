# language: en

Feature: Sending reports

  Steering reports should be published to the control CAN bus after an interval.


  Scenario: Heartbeat report published after an interval
    When the time since the last heartbeat report publishing exceeds the interval

    Then a heartbeat report should be published to the control CAN bus


  Scenario: Chassis State 1 report published after an interval
    When the time since the last Chassis State 1 report publishing exceeds the interval

    Then a Chassis State 1 report should be published to the control CAN bus


  Scenario: Chassis State 2 report published after an interval
    When the time since the last Chassis State 2 report publishing exceeds the interval

    Then a Chassis State 2 report should be published to the control CAN bus
