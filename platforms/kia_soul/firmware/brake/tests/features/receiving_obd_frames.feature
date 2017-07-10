# language: en

Feature: Receiving OBD frames

  OBD frames should be received and parsed.


  Scenario Outline: Brake pressure OBD frame sent from CAN gateway.
    When a brake pressure OBD frame is received with brake pressure <pressure>

    Then the control state's current_vehicle_brake_pressure field should be <pressure>

    Examples:
        | pressure |
        |  -32768  |
        |  -16384  |
        |  -8192   |
        |  -4096   |
        |  -2048   |
        |  -1024   |
        |  -512    |
        |  -256    |
        |  0       |
        |  256     |
        |  512     |
        |  1024    |
        |  2048    |
        |  4096    |
        |  8192    |
        |  16348   |
        |  32767   |
