# Consensus

The purpose of this Python 3 program is to provide basic validation that there is the vehicle and
OSCC can reach a consensus. Ideally it's a method to quickly verify whether your vehicle is in a
good state. By sending control commands on the CAN gateway then polling for an expected result
there, we should be able to tell whether OSCC is functioning properly or diagnose any issues more
readily.

While this program was designed to be run as a stand alone executable it may be imported as a module
should a use-case arise.

Prerequisites for success:

- A vehicle with OSCC installed
- That vehicle powered on (motor on)
- OSCC powered
- A socketcan connection to OSCC's CAN bus from your computer

But what does it do?

1. Enable each OSCC module (brake, steering, throttle).
1. Send commands to increase and decrease brake pressure.
1. Verify that brake pressure reported by vehicle increased or decreased accordingly.
1. Send commands to apply positive or negative torque to steering wheel.
1. Verify that the steering wheel angle increased or decreased accordingly.
1. Send commands to increase or decrease throttle pressure.
1. Verify just that the wheel position is reported. (This one's unique in that we don't require the
   wheel positions to expect as a success condition. They shouldn't if the car's in park!.
1. Disable each OSCC module.

## Dependencies

- `python3`
- `pip`
- `python-can` (run `pip install python-can`)
- `docopt` (run `pip install docopt`)

## Usage

Connect your computer to OSCC's CAN bus and bring up your socketcan interface

```bash
sudo ip link set can0 type can bitrate 125000
sudo ip link set up can0
```

Run `consensus.py --help` for more info.
