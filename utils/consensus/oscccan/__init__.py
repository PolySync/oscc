"""
Import as 'oscccan'
"""

__all__ = [
    'canbus',
    ]

class OsccModule(object):
    """
    Wrapper to CAN data specific to an OSCC module. Used with CanBus class to
    communicate on the OSCC CAN bus.
    """
    def __init__(self, base_arbitration_id, module_name=None):
        """
        Initialize CAN data specific to OSCC module.
        """
        try:
            int(base_arbitration_id)
        except:
            raise ValueError(
                'unable to represent given base_arbitration_id as an integer: ',
                base_arbitration_id)

        self.magic_word = [0x05, 0xCC]
        self.enable_arbitration_id = base_arbitration_id
        self.disable_arbitration_id = base_arbitration_id + 1
        self.command_arbitration_id = base_arbitration_id + 2
        self.report_arbitration_id = base_arbitration_id + 3
        self.module_name = module_name
