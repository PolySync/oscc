"""
Import as 'canbus' from 'oscccan'. Contains 'CanBus' class used for communicating on the OSCC CAN bus
as well as the 'Report' class that the CanBus class's command methods return.
"""

# This is the module required for everything CAN bus. It is not default on most systems though
# and needs to be installed with something like `pip install python-can`
import can
# The `struct` module is really helpful for packing bytes into messages for transmission on the CAN
# bus as well as unpacking bytes into an object for this program's consumption.
import struct
# This module lets us sleep intermittently. We're not in a hurry and want to see how the car behaves
# when commands are spaced out a bit.
import time

from oscccan import OsccModule

class Report(object):
    """
    Class returned by the command methods of the 'CanBus' class.
    """
    def __init__(self, success=None, value=None):
        self.success = success
        self.value = value

class CanBus(object):
    """
    CanBus class for to connecting to a CAN bus and communicating with OSCC modules.
    """
    def __init__(
        self,
        bustype='socketcan_native',
        channel='can0',
        bitrate=500000,
        vehicle='kia_soul_ev'):
        """
        Connect to CAN bus.
        """

        try:
            self.bus = can.interface.Bus(
                bustype=bustype,
                channel=channel,
                bitrate=bitrate)
        except:
            raise Exception(
                'unable to connect to CAN bus, check that hardware '
                'is connected and that socketcan is active')

        self.brake_pressure_arbitration_ids = [0x220]
        self.steering_wheel_angle_arbitration_ids = [0x2B0]
        self.wheel_speed_arbitration_ids = [0x4B0, 0x386]
        self.vehicle = vehicle

    def bus_send_msg(self, arbitration_id, data=None, timeout=1.0):
        """
        Send a frame on OSCC CAN bus.
        """

        msg = can.Message(
            arbitration_id=arbitration_id,
            data=data)
        self.bus.send(msg, timeout=timeout)

    def enable_module(self, module, timeout=None):
        """
        Send enable command specific to the module parameter.
        """
        if not isinstance(module, OsccModule):
            raise TypeError('cannot enable', module)

        self.bus_send_msg(
            arbitration_id=module.enable_arbitration_id,
            data=module.magic_word + [0, 0, 0, 0, 0, 0],
            timeout=timeout
        )

    def disable_module(self, module, timeout=None):
        """
        Send disable command specific to the module parameter.
        """
        if not isinstance(module, OsccModule):
            raise TypeError('cannot disable', module)

        self.bus_send_msg(
            arbitration_id=module.disable_arbitration_id,
            data=module.magic_word + [0, 0, 0, 0, 0, 0],
            timeout=timeout
        )

    def check_module_enabled_status(
        self,
        module,
        timeout=1.0,
        expect=False):
        """
        Check if OSCC module is enabled/disabled.
        """
        if not isinstance(module, OsccModule):
            raise TypeError('cannot check status for', module)

        status = False
        wait = time.time() + timeout

        while True:
            if time.time() > wait:
                break

            msg = self.recv_report(module=module, timeout=timeout)

            if msg is not None:
                byte_lst = list(msg.data)
                if byte_lst[2] != 0 and expect is True:
                    status = True
                    break

                if byte_lst[2] == 0 and expect is False:
                    status =  True
                    break

        return status

    def send_command(
        self,
        module,
        value,
        timeout=None):
        """
        Send control command specifed by floating point value to the OsccModule parameter.
        """
        if not isinstance(module, OsccModule):
            raise TypeError('cannot send command to', module)

        try:
            float(value)
        except:
            raise ValueError('invalid command', value)

        byte_list = list(bytearray(struct.pack("f", value)))

        self.bus_send_msg(
            arbitration_id=module.command_arbitration_id,
            data=module.magic_word + byte_list + [0, 0],
            timeout=timeout
        )

    def recv_report(
        self,
        module=None,
        can_ids=None,
        timeout=1.0):
        """
        If OsccModule parameter is valid, return its report message.
        If OsccModule invalid and can_id is valid, return message with that ID.
        If both OsccModule and can_id are invalid, return first message received.
        """
        msg_ids = []
        if can_ids is not None:
            if not isinstance(can_ids, list):
                msg_ids.append(can_ids)
            else:
                msg_ids.extend(can_ids)

        # throw away can_id if module parameter is valid
        if module is not None:
            if not isinstance(module, OsccModule):
                raise TypeError('cannot find CAN ID in', module)
            msg_ids.append(module.report_arbitration_id)

        wait = time.time() + timeout

        while True:
            msg = self.bus.recv(timeout)
            if msg is not None:
                if msg.arbitration_id in msg_ids:
                    return msg
                elif can_ids is None and module is None:
                    return msg
            else:
                return None

            if wait > time.time():
                return None

    def check_brake_pressure(
        self,
        increase_from=None,
        decrease_from=None,
        timeout=2.0,):
        """
        Check brake pressure report from vehicle. If the increase_from or decrease_from parameters
        are populated, verify the reported value did increase or decrease.
        """
        value = None
        wait = time.time() + timeout

        while True:
            if time.time() > wait:
                if increase_from is None and decrease_from is None:
                    value = None
                return Report(success=False, value=value)

            msg = self.recv_report(can_ids=self.brake_pressure_arbitration_ids, timeout=timeout)

            if msg is None:
                continue

            if self.vehicle == 'kia_niro':
                # Niro
                byte1 = (msg.data[4] & 0x0F) << 8
                byte0 = msg.data[3]
                value = int(str(byte1|byte0), 10)
                value /= 40
            else:
                # Soul EV and Petrol
                byte1 = (msg.data[5] & 0x0F) << 8
                byte0 = msg.data[4]
                value = int(str(byte1|byte0), 10)
                value /= 10

            if increase_from is not None:
                if value > increase_from:
                    return Report(success=True, value=value)
            elif decrease_from is not None:
                if value < decrease_from:
                    return Report(success=True, value=value)
            else:
                return Report(success=True, value=value)

    def check_steering_wheel_angle(
        self,
        increase_from=None,
        decrease_from=None,
        timeout=2.0):
        """
        Check steering wheel angle report from vehicle. If the increase_from or decrease_from
        parameters are populated, verify the reported value did increase or decrease.
        """
        value = None
        wait = time.time() + timeout

        while True:
            if time.time() > wait:
                if increase_from is None and decrease_from is None:
                    value = None
                return Report(success=False, value=value)

            msg = self.recv_report(
                can_ids=self.steering_wheel_angle_arbitration_ids,
                timeout=timeout)

            if msg is None:
                continue

            value = -float(struct.unpack("h", msg.data[:2])[0]) / 10.0

            if increase_from is not None:
                if value > increase_from:
                    return Report(success=True, value=value)
            elif decrease_from is not None:
                if value < decrease_from:
                    return Report(success=True, value=value)
            else:
                return Report(success=True, value=value)

    def get_wheel_speed(self, data, offset):
        """
        Wheel speed unpacking logic generic across offsets.
        """
        byte1 = (data[offset + 1] & 0x0F) << 8
        byte0 = data[offset]
        value = int(str(byte1|byte0), 10)

        return float(value) / 10.0

    def check_wheel_speed(
        self,
        timeout=2.0):
        """
        Check wheel speed report from vehicle.
        """

        wait = time.time() + timeout

        while True:
            if time.time() > wait:
                return Report(success=False, value=None)

            msg = self.recv_report(can_ids=self.wheel_speed_arbitration_ids, timeout=timeout)

            if msg is not None:
                left_front = None
                right_front = None
                left_rear = None
                right_rear = None

                left_front = self.get_wheel_speed(msg.data, 0)
                right_front = self.get_wheel_speed(msg.data, 2)
                left_rear = self.get_wheel_speed(msg.data, 4)
                right_rear = self.get_wheel_speed(msg.data, 6)

                return Report(success=True, value=[left_front, right_front, left_rear, right_rear])
