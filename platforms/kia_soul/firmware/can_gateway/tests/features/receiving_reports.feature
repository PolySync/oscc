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

  Scenario Outline: Engine RPM sent from Vehicle.
    When an engine report is sent from the vehicle with the RPM <rpm>, Temperature <temp>

    Then the Engine RPM/Temp heartbeat warning should be cleared
    And the Engine RPM should be <rpm>
    And the Engine Temperature should <temp>

    Examples:
        | rpm | temperature | 
        | 0   |  80         | 
        | 100 |  100        | 
        | 500 |  120        | 

  Scenario Outline: Vehicle Speed sent from Vehicle.
    When an Vehicle Speed is sent from the vehicle with the Vehicle Speed <speed>

    Then the Vehicle Speed heartbeat warning should be cleared
    And the vehicle speed should be <speed>

    Examples:
        | speed |  
        | 0     | 
        | 100   | 
        | 150   | 

  Scenario Outline: Gear Position sent from Vehicle.
    When an Gear Position is sent from the vehicle with the Gear Position <pos>

    Then the Gear Position heartbeat warning should be cleared
    And the gear position should be set

    Examples:
        | pos  |  value
        | 3    |   P
        | 5    |   R
        | 9    |   N
        | 11   |   D


  Scenario Outline: Accelerator Pedal Position sent from Vehicle.
    When an Accelerator Pedal is sent from the vehicle with the Accelerator Pedal Position <pos>

    Then the Accelerator Pedal Position heartbeat warning should be cleared
    And the accelerator pedal position should be set

    Examples:
        | pos   |  
        | 0     |  
        | 10    | 
        | 20    | 
        | 50    |  


  

