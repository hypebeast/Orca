# Copyright (C) 2012 Sebastian Ruml <sebastian.ruml@gmail.com>
#
# This file is part of the Matunus project (part of the orcacopter project)
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 1, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

__author__ = 'Sebastian Ruml'

try:
    from PyQt4.QtCore import QObject
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)

import struct

from crc8 import crc8
from logger import Logger


class CommandTypes:
    """
    This class defines the API command types.
    """
    SET_SERVO_POS = 0x0001
    GET_SERVO_POS = 0x0002
    GET_ALL_SERVO_POS = 0x0003
    GET_BOARD_STATUS = 0x0010
    GET_BOARD_SETTINGS = 0x0020
    SAVE_BOARD_SETTINGS = 0x0021
    SET_ROLL_PID_COEFFICIENTS = 0x0022
    SET_PITCH_PID_COEFFICIENTS = 0x0023
    SET_YAW_PID_COEFFICIENTS = 0x0024
    SET_KALMAN_ROLL_CONSTANTS = 0x0025
    SET_KALMAN_PITCH_CONSTANTS = 0x0026
    SET_KALMAN_YAW_CONSTANTS = 0x0027
    DEBUG_INT_VALUES = 0x0100
    DEBUG_FLOAT_VALUES = 0x0101
    DEBUG_STRING_MESSAGE = 0x0102


class MessageTypes:
    """
    Message types.
    """
    COMMAND_MESSAGE = 0x10
    RESPONSE_MESSAGE = 0x20


class MessageFactory:
    """
    Factory for messages.
    """
    @staticmethod
    def getMessage(data):
        """This message returns an API message from the given data array"""
        def getCommandType(data):
            if data is None or len(data) < 2:
                return None

            return struct.unpack_from("H", data, 2)[0]
            
        # Logger
        logger = Logger()

        # Get the command type
        command_type = getCommandType(data)

        message = None
        if command_type == CommandTypes.GET_SERVO_POS:
            message = GetServoPositionMessage().fromPacket(data)
        elif command_type == CommandTypes.GET_ALL_SERVO_POS:
            pass
        elif command_type == CommandTypes.GET_BOARD_STATUS:
            message = GetBoardStatusMessage().fromPacket(data)
        elif command_type == CommandTypes.GET_BOARD_SETTINGS:
            message = GetBoardSettingsMessage().fromPacket(data)
        elif command_type == CommandTypes.DEBUG_INT_VALUES:
            message = DebugIntValuesMessage().fromPacket(data)
        elif command_type == CommandTypes.DEBUG_FLOAT_VALUES:
            message = DebugFloatValuesMessage().fromPacket(data)
        elif command_type == CommandTypes.DEBUG_STRING_MESSAGE:
            message = DebugStringMessage().fromPacket(data)
        else:
            logger.warn("Message type not found")    

        return message


class BaseMessage(QObject):
    """
    Base class for an command message packet. An command message is used for
    the communication with the flight controller over a serial connection.
    """

    # Constants
    START_BYTE = 0x8D
    STOP_BYTE = 0x7E

    # Type tables used for encoding and decoding of the message data
    SUPPORTED_TYPES = ["B", "H", "f"]
    TYPE_SIZES = [  
                    1, # unsigned char (B)
                    2, # unsigned short (H)
                    4  # float (f)
                ]

    def __init__(self, commandType):
        QObject.__init__(self)
        self.commandType = commandType
        self.messageType = MessageTypes.COMMAND_MESSAGE

    def getPacket(self):
        """
        This method returns a byte array which can be send via a serial connection.
        Every class that inherets this class must override this method.
        """
        pass

    @staticmethod
    def fromPacket(package):
        """
        Creates a new message object from the given package.
        Every class that inherets this class must override this method
        """
        pass

    def _encodePackage(self, data):
        """
        Returns the encoded message as a byte array.
        
        Params:
        data: An array of dictionaries which contains the arguments and their corresponding format character
              Format = [{"value": value, "format": format character}]
        """
        package = bytearray()

        # Start byte
        package.extend(struct.pack("B", self.START_BYTE))
        # Message type
        package.extend(struct.pack("B", self.messageType))
        # Command type
        package.extend(struct.pack("H", self.commandType))
        # Data length
        package.extend(struct.pack("B", self._calculateDataLength(data)))
        # Encode the data
        self._packData(package, data)
        # CRC
        package.extend(struct.pack("B", self._calcCrc(package, data)))
        # Stop Byte
        package.extend(struct.pack("B", self.STOP_BYTE))

        return package

    def _decodePackage(self, data):
        pass

    def _calculateDataLength(self, data):
        """
        This method calculates the length of the data and returns the length
        in bytes.
        """
        if len(data) < 1:
            return 0

        data_length = 0
        for argument in data:
            try:
                index = self.SUPPORTED_TYPES.index(argument["format"])
                data_length = data_length + self.TYPE_SIZES[index]
            except Exception as detail:
                print detail

        return data_length

    def _packData(self, package, data):
        """Encodes the data and returns the package with the encoded data."""
        if len(data) < 1:
            return

        # Encode all arguments
        for argument in data:
            package.extend(struct.pack(argument["format"], argument["value"]))

    def _calcCrc(self, package, data):
        """Calculates the CRC8 for the given package and returns it."""
        msg = bytearray()

        msg.extend(struct.pack("B", self.START_BYTE))
        msg.extend(struct.pack("B", self.messageType))
        msg.extend(struct.pack("H", self.commandType))
        self._packData(msg, data)

        crc = crc8()
        return crc.crc(msg)


class ServoPositionMessage(BaseMessage):
    """
    This command sets a new position to the specified servo.

    Command Type = 0x0001
    """
    def __init__(self, servo_nr = 1, position = 0):
        BaseMessage.__init__(self, CommandTypes.SET_SERVO_POS)
        self.messageType = MessageTypes.COMMAND_MESSAGE

        self.servo_nr = servo_nr
        self.position = position

    def getPacket(self):
       data = []
       data.append({"value": self.servo_nr, "format": "B"})
       data.append({"value": self.position, "format": "H"})

       return self._encodePackage(data)


class GetServoPositionMessage(BaseMessage):
    """
    This command requests the current position of servo channel.

    Command Type: 0x0002
    """
    def __init__(self, servo_nr = 1):
        BaseMessage.__init__(self, CommandTypes.GET_SERVO_POS)
        self.messageType = MessageTypes.COMMAND_MESSAGE

        self.servo_nr = servo_nr
        self.position = 0.0

    def getPacket(self):
        data = []
        data.append({"value": self.servo_nr, "format": "B"})

        return self._encodePackage(data)

    @staticmethod
    def fromPacket(package):
        # Get the command type
        command_type = struct.unpack_from("H", package, 2)

        if command_type[0] is not CommandTypes.GET_SERVO_POS:
            return None

        # Parse the data
        servo_nr = struct.unpack_from("B", package, 5)[0]
        position = struct.unpack_from("H", package, 6)[0]

        message = GetServoPositionMessage(servo_nr)
        message.messageType = MessageTypes.RESPONSE_MESSAGE
        message.position = position

        return message


class GetBoardStatusMessage(BaseMessage):
    """
    Board status message (Command Type: 0x0010)
    """
    def __init__(self):
        BaseMessage.__init__(self, CommandTypes.GET_BOARD_STATUS)
        self.messageType = MessageTypes.COMMAND_MESSAGE

        self.outputChannel1 = 0
        self.outputChannel2 = 0
        self.outputChannel3 = 0
        self.outputChannel4 = 0
        self.outputChannel5 = 0
        self.outputChannel6 = 0
        self.inputChannel1 = 0
        self.inputChannel2 = 0
        self.inputChannel3 = 0
        self.inputChannel4 = 0
        self.inputChannel5 = 0
        self.inputChannel6 = 0
        self.inputChannel7 = 0
        self.accelerationX = 0
        self.accelerationY = 0
        self.accelerationZ = 0
        self.gyroX = 0
        self.gyroY = 0
        self.gyroZ = 0
        self.sensorRollAngle = 0
        self.sensorPitchAngle = 0
        self.sensorYawAngle = 0
        self.dcmOutputRoll = 0
        self.dcmOutputPitch = 0
        self.dcmOutputYaw = 0
        self.setRollAngle = 0
        self.setPitchAngle = 0
        self.setYawAngle = 0
        self.pidRollActuatingValue = 0
        self.pidPitchActuatingValue = 0
        self.pidYawActuatingValue = 0

    def getPacket(self):
        return self._encodePackage([])

    @staticmethod
    def fromPacket(package):
        #hexvalues = ''.join('%02x' % ord(byte) for byte in package)
        #print "Board status message: " + hexvalues

        # Check for the right command type
        command_type = struct.unpack_from("H", package, 2)
        if command_type[0] is not CommandTypes.GET_BOARD_STATUS:
            return None

        message = GetBoardStatusMessage()
        message.messageType = MessageTypes.RESPONSE_MESSAGE

        #print "Message length: " + str(len(package))

        # FIXME: Quick hack to filter out malformed packages
        if len(package) != 105:
            return None

        # Parse the data
        offset = 5
        try:
            message.outputChannel1 = struct.unpack_from("H", package, offset)[0]
            offset += 2
            message.outputChannel2 = struct.unpack_from("H", package, offset)[0]
            offset += 2
            message.outputChannel3 = struct.unpack_from("H", package, offset)[0]
            offset += 2
            message.outputChannel4 = struct.unpack_from("H", package, offset)[0]
            offset += 2
            message.outputChannel5 = struct.unpack_from("H", package, offset)[0]
            offset += 2
            message.outputChannel6 = struct.unpack_from("H", package, offset)[0]
            offset += 2
            message.inputChannel1 = struct.unpack_from("H", package, offset)[0]
            offset += 2
            message.inputChannel2 = struct.unpack_from("H", package, offset)[0]
            offset += 2
            message.inputChannel3 = struct.unpack_from("H", package, offset)[0]
            offset += 2
            message.inputChannel4 = struct.unpack_from("H", package, offset)[0]
            offset += 2
            message.inputChannel5 = struct.unpack_from("H", package, offset)[0]
            offset += 2
            message.inputChannel6 = struct.unpack_from("H", package, offset)[0]
            offset += 2
            message.inputChannel7 = struct.unpack_from("H", package, offset)[0]
            offset += 2
            message.accelerationX = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.accelerationY = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.accelerationZ = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.gyroX = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.gyroY = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.gyroZ = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.sensorRollAngle = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.sensorPitchAngle = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.sensorYawAngle = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.dcmOutputRoll = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.dcmOutputPitch = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.dcmOutputYaw = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.setRollAngle = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.setPitchAngle = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.setYawAngle = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.pidRollActuatingValue = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.pidPitchActuatingValue = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.pidYawActuatingValue = struct.unpack_from("f", package, offset)[0]
            offset += 4
        except:
            print "Unpacking Error"

        return message


class GetBoardSettingsMessage(BaseMessage):
    """
    This message type requests the board settings (Command Type: 0x0020)
    """
    def __init__(self):
        BaseMessage.__init__(self, CommandTypes.GET_BOARD_SETTINGS)
        self.messageType = MessageTypes.COMMAND_MESSAGE

        self.pidRollPFactor = 0
        self.pidRollIFactor = 0
        self.pidRollDFactor = 0
        self.pidRollILimit = 0
        self.pidPitchPFactor = 0
        self.pidPitchIFactor = 0
        self.pidPitchDFactor = 0
        self.pidPitchILimit = 0
        self.pidYawPFactor = 0
        self.pidYawIFactor = 0
        self.pidYawDFactor = 0
        self.pidYawILimit = 0
        self.dcmRollPFactor = 0
        self.dcmRollIFactor = 0
        self.dcmPitchPFactor = 0
        self.dcmPitchIFactor = 0
        self.dcmYawPFactor = 0
        self.dcmYawIFactor = 0

    def getPacket(self):
        return self._encodePackage([])

    @staticmethod
    def fromPacket(package):
         # Check if we received a message with ID 0x0020
        command_type = struct.unpack_from("H", package, 2)
        if command_type[0] is not CommandTypes.GET_BOARD_SETTINGS:
            return None

        message = GetBoardSettingsMessage()
        message.messageType = MessageTypes.RESPONSE_MESSAGE

        # Parse the data
        offset = 5
        try:
            message.pidRollPFactor = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.pidRollIFactor = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.pidRollDFactor = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.pidRollILimit = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.pidPitchPFactor = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.pidPitchIFactor = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.pidPitchDFactor = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.pidPitchILimit = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.pidYawPFactor = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.pidYawIFactor = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.pidYawDFactor = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.pidYawILimit = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.dcmRollPFactor = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.dcmRollIFactor = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.dcmPitchPFactor = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.dcmPitchIFactor = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.dcmYawPFactor = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.dcmYawIFactor = struct.unpack_from("f", package, offset)[0]
            offset += 4
        except:
            pass

        return message


class SaveSettingsMessage(BaseMessage):
    """
    Saves the FMU settings on the FMU (Command Type: 0x0021).
    """
    def __init__(self):
        BaseMessage.__init__(self, CommandTypes.SAVE_BOARD_SETTINGS)
        self.messageType = MessageTypes.COMMAND_MESSAGE

    def getPacket(self):
        return self._encodePackage([])

    @staticmethod
    def fromPacket(package):
        raise NotImplementedError


class SetRollPIDCoefficientsMessage(BaseMessage):
    """
    Sets the PID coeficients for the roll axis (Command Type: 0x0022).
    """
    def __init__(self, p_factor, i_factor, d_factor, i_limit):
        BaseMessage.__init__(self, CommandTypes.SET_ROLL_PID_COEFFICIENTS)
        self.messageType = MessageTypes.COMMAND_MESSAGE

        self.pFactor = p_factor
        self.iFactor = i_factor
        self.dFactor = d_factor
        self.iLimit = i_limit

    def getPacket(self):
        data = []
        data.append({"value": self.pFactor, "format": "f"})
        data.append({"value": self.iFactor, "format": "f"})
        data.append({"value": self.dFactor, "format": "f"})
        data.append({"value": self.iLimit, "format": "f"})

        return self._encodePackage(data)

    @staticmethod
    def fromPacket(package):
        raise NotImplementedError


class SetPitchPIDCoefficientsMessage(BaseMessage):
    """
    Sets the PID coeficients for the pitch axis (Command Type: 0x0023)
    """
    def __init__(self, p_factor, i_factor, d_factor, i_limit):
        BaseMessage.__init__(self, CommandTypes.SET_PITCH_PID_COEFFICIENTS)
        self.messageType = MessageTypes.COMMAND_MESSAGE

        self.pFactor = p_factor
        self.iFactor = i_factor
        self.dFactor = d_factor
        self.iLimit = i_limit

    def getPacket(self):
        data = []
        data.append({"value": self.pFactor, "format": "f"})
        data.append({"value": self.iFactor, "format": "f"})
        data.append({"value": self.dFactor, "format": "f"})
        data.append({"value": self.iLimit, "format": "f"})

        return self._encodePackage(data)

    @staticmethod
    def fromPacket(package):
        raise NotImplementedError


class SetYawPIDCoefficientsMessage(BaseMessage):
    """
    Sets the PID coeficients for the yaw axis (Command Type: 0x0024)
    """
    def __init__(self, p_factor, i_factor, d_factor, i_limit):
        BaseMessage.__init__(self, CommandTypes.SET_YAW_PID_COEFFICIENTS)
        self.messageType = MessageTypes.COMMAND_MESSAGE

        self.pFactor = p_factor
        self.iFactor = i_factor
        self.dFactor = d_factor
        self.iLimit = i_limit

    def getPacket(self):
        data = []
        data.append({"value": self.pFactor, "format": "f"})
        data.append({"value": self.iFactor, "format": "f"})
        data.append({"value": self.dFactor, "format": "f"})
        data.append({"value": self.iLimit, "format": "f"})

        return self._encodePackage(data)

    @staticmethod
    def fromPacket(package):
        raise NotImplementedError


class SetDcmRollCoefficientsMessage(BaseMessage):
    """
    Sets the Kalman constants for the roll axis (Command Type: 0x0025)
    """
    def __init__(self, p_factor, i_factor):
        BaseMessage.__init__(self, CommandTypes.SET_KALMAN_ROLL_CONSTANTS)
        self.messageType = MessageTypes.COMMAND_MESSAGE

        self.p_factor = p_factor
        self.i_factor = i_factor

    def getPacket(self):
        data = []
        data.append({"value": self.p_factor, "format": "f"})
        data.append({"value": self.i_factor, "format": "f"})

        return self._encodePackage(data)

    @staticmethod
    def fromPacket(package):
        raise NotImplementedError


class SetDcmPitchCoefficientsMessage(BaseMessage):
    """
    Sets the Kalman constants for the pitch axis (Command Type: 0x0026)
    """
    def __init__(self, p_factor, i_factor):
        BaseMessage.__init__(self, CommandTypes.SET_KALMAN_PITCH_CONSTANTS)
        self.messageType = MessageTypes.COMMAND_MESSAGE

        self.p_factor = p_factor
        self.i_factor = i_factor

    def getPacket(self):
        data = []
        data.append({"value": self.p_factor, "format": "f"})
        data.append({"value": self.i_factor, "format": "f"})

        return self._encodePackage(data)

    @staticmethod
    def fromPacket(package):
        raise NotImplementedError


class SetDcmYawCoefficientsMessage(BaseMessage):
    """
    Sets the Kalman constants for the pitch axis (Command Type: 0x0027)
    """
    def __init__(self, p_factor, i_factor):
        BaseMessage.__init__(self, CommandTypes.SET_KALMAN_YAW_CONSTANTS)
        self.messageType = MessageTypes.COMMAND_MESSAGE

        self.p_factor = p_factor
        self.i_factor = i_factor

    def getPacket(self):
        data = []
        data.append({"value": self.p_factor, "format": "f"})
        data.append({"value": self.i_factor, "format": "f"})

        return self._encodePackage(data)

    @staticmethod
    def fromPacket(package):
        raise NotImplementedError


class DebugIntValuesMessage(BaseMessage):
    """
    Debug integer values message (Command Type: 0x0100)
    """
    def __init__(self):
        BaseMessage.__init__(self, CommandTypes.DEBUG_INT_VALUES)
        self.messageType = MessageTypes.RESPONSE_MESSAGE

        self.value1 = 0
        self.value2 = 0
        self.value3 = 0
        self.value4 = 0
        self.value5 = 0

    def getPacket(self):
        return self._encodePackage([])

    @staticmethod
    def fromPacket(package):
         # Check if we received a message with ID 0x0100
        command_type = struct.unpack_from("H", package, 2)
        if command_type[0] != CommandTypes.DEBUG_INT_VALUES:
            return None

        message = DebugIntValuesMessage()
        message.messageType = MessageTypes.RESPONSE_MESSAGE

        # Parse the data
        offset = 5
        try:
            message.value1 = struct.unpack_from("H", package, offset)[0]
            offset += 2
            message.value2 = struct.unpack_from("H", package, offset)[0]
            offset += 2
            message.value3 = struct.unpack_from("H", package, offset)[0]
            offset += 2
            message.value4 = struct.unpack_from("H", package, offset)[0]
            offset += 2
            message.value5 = struct.unpack_from("H", package, offset)[0]
        except:
            pass

        return message


class DebugFloatValuesMessage(BaseMessage):
    """
    Debug float values message (Command Type: 0x0101)
    """
    def __init__(self):
        BaseMessage.__init__(self, CommandTypes.DEBUG_FLOAT_VALUES)
        self.messageType = MessageTypes.RESPONSE_MESSAGE

        self.value1 = 0
        self.value2 = 0
        self.value3 = 0
        self.value4 = 0
        self.value5 = 0

    def getPacket(self):
        return self._encodePackage([])

    @staticmethod
    def fromPacket(package):
         # Check if we received a message with ID 0x0101
        command_type = struct.unpack_from("H", package, 2)
        if command_type[0] != CommandTypes.DEBUG_FLOAT_VALUES:
            return None

        message = DebugFloatValuesMessage()
        message.messageType = MessageTypes.RESPONSE_MESSAGE

        # Parse the data
        offset = 5
        try:
            message.value1 = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.value2 = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.value3 = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.value4 = struct.unpack_from("f", package, offset)[0]
            offset += 4
            message.value5 = struct.unpack_from("f", package, offset)[0]
        except:
            pass

        return message


class DebugStringMessage(BaseMessage):
    """
    Debug float values message (Command Type: 0x0101)
    """
    def __init__(self):
        BaseMessage.__init__(self, CommandTypes.DEBUG_STRING_MESSAGE)
        self.messageType = MessageTypes.RESPONSE_MESSAGE

        self.message = ""

    def getPacket(self):
        return self._encodePackage([])

    @staticmethod
    def fromPacket(package):
         # Check if we received a message with ID 0x0102
        command_type = struct.unpack_from("H", package, 2)
        if command_type[0] != CommandTypes.DEBUG_STRING_MESSAGE:
            return None

        message = DebugStringMessage()
        message.messageType = MessageTypes.RESPONSE_MESSAGE

        # Get the data length
        length = struct.unpack_from("B", package, 4)
        length = length[0] + 1
        fmtString = "%ip" % length

        # Parse the data
        offset = 4
        try:
            message.message = struct.unpack_from(fmtString, package, offset)[0]
        except:
            pass

        return message