# language: en

Feature: Receiving reports

  Chassis state reports should be received and parsed.


  Scenario Outline: Chassis State 1 report sent from CAN gateway.
    When a Chassis State 1 report is received with steering wheel angle <raw_angle>

    Then the control state's current_steering_wheel_angle field should be <scaled_angle>

    Examples:
        | raw_angle | scaled_angle |
        |  -32768   |  -2925.00    |
        |  -16384   |  -1462.50    |
        |  -8192    |  -731.250    |
        |  -4096    |  -365.625    |
        |  -2048    |  -182.812    |
        |  -1024    |  -91.4063    |
        |  -512     |  -45.7031    |
        |  -256     |  -22.8515    |
        |  0        |  0.0         |
        |  256      |  22.8515     |
        |  512      |  45.7031     |
        |  1024     |  91.4063     |
        |  2048     |  182.812     |
        |  4096     |  365.625     |
        |  8192     |  731.250     |
        |  16348    |  1459.28     |
        |  32767    |  2924.91     |

