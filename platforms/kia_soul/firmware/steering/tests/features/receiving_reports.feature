# language: en

Feature: Receiving reports

  Chassis state reports should be received and parsed.


  Scenario Outline: Chassis State 1 report sent from CAN gateway.
    When a Chassis State 1 report is received with steering wheel angle <raw_angle>

    Then the control state's current_steering_wheel_angle field should be <scaled_angle>

    Examples:
        | raw_angle | scaled_angle |
        |  -32768   |  -2925       |
        |  -16384   |  -1462       |
        |  -8192    |  -731        |
        |  -4096    |  -365        |
        |  -2048    |  -182        |
        |  -1024    |  -91         |
        |  -512     |  -45         |
        |  -256     |  -22         |
        |  0        |  0           |
        |  256      |  22          |
        |  512      |  45          |
        |  1024     |  91          |
        |  2048     |  182         |
        |  4096     |  365         |
        |  8192     |  731         |
        |  16348    |  1459        |
        |  32767    |  2924        |

