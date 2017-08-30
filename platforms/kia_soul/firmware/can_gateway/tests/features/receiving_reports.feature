# language: en

Feature: Receiving reports

  OBD reports should be received and parsed.


  Scenario Outline: Steering wheel angle report sent from vehicle.
    When a steering wheel angle report is sent from the vehicle with the steering wheel angle <angle>

    Then the steering wheel angle heartbeat warning should be cleared
    And the steering wheel angle rate valid flag should be cleared
    And the steering wheel angle valid flag should be set
    And the Chassis State 1 steering wheel angle field should be set to <angle>
    And the Chassis State 1 steering wheel angle rate field should be set to 0
    And the last received steering wheel angle timestamp should be set

    Examples:
        | angle |
        |  -256 |
        |  -128 |
        |  0    |
        |  128  |
        |  256  |


  Scenario Outline: Wheel speed report sent from vehicle.
    When a wheel speed report is sent from the vehicle with the wheel speed <speed>

    Then the wheel speed heartbeat warning should be cleared
    And the wheel speed valid flag should be set
    And the Chassis State 2 wheel speed fields should be set to <speed>
    And the last received wheel speed timestamp should be set

    Examples:
        | speed |
        |  -256 |
        |  -128 |
        |  0    |
        |  128  |
        |  256  |


  Scenario Outline: Brake pressure report sent from vehicle.
    When a brake pressure report is sent from the vehicle with the brake pressure <pressure>

    Then the brake pressure heartbeat warning should be cleared
    And the brake pressure valid flag should be set
    And the Chassis State 1 brake pressure field should be set to <pressure>
    And the last received brake pressure timestamp should be set

    Examples:
        | pressure |
        |  -256    |
        |  -128    |
        |  0       |
        |  128     |
        |  256     |


  Scenario Outline: Turn signal report sent from vehicle.
    When a turn signal report is sent from the vehicle with the turn signal <signal>

    Then the turn signal heartbeat warning should be cleared
    And the left turn signal flag should be <left_flag>
    And the right turn signal flag should be <right_flag>
    And the brake signal flag should be <brake_flag>

    Examples:
        | signal | left_flag | right_flag | brake_flag |
        |  left  |  set      |  cleared   |  cleared   |
        |  right |  cleared  |  set       |  cleared   |

  Scenario Outline: Engine report sent from vehicle.
    When an engine report is sent from the vehicle with the rpm <rpm> and temperature <temp>

    Then the engine report heartbeat warning should be cleared
    Then the Chassis State 3 engine rpm is <rpm> and temperature is <temp>
    Then the last received engine report timestamp should be set

    Examples:
        | rpm |  temp       | 
        | 0   |  80         | 
        | 100 |  100        | 
        | 500 |  120        | 

  Scenario Outline: Vehicle speed sent from vehicle.
    When a vehicle speed report is sent from the vehicle with the vehicle speed <speed>

    Then the vehicle speed heartbeat warning should be cleared
    Then the Chassis State 3 vehicle speed field should be set to <speed>
    Then the last received vehicle speed timestamp should be set

    Examples:
        | speed |  
        | 0     | 
        | 100   | 
        | 150   | 

  Scenario Outline: Gear position sent from vehicle.
    When a gear position report is sent from the vehicle with the gear position <pos>

    Then the gear position heartbeat warning should be cleared
    Then the Chassis State 3 gear position field should be set to <pos>
    Then the last received gear position timestamp should be set

    Examples:
        | pos  |
        | 3    |
        | 5    |
        | 9    |
        | 11   |


  Scenario Outline: Accelerator pedal position sent from vehicle.
    When an accelerator pedal position report is sent from the vehicle with the position <pos>

    Then the accelerator pedal position heartbeat warning should be cleared
    Then the Chassis State 3 accelerator pedal position field should be set to <pos>
    Then the last received accelerator pedal position timestamp should be set

    Examples:
        | pos   |  
        | 0     |  
        | 10    | 
        | 20    | 
        | 50    |  


  

