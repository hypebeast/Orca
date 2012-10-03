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


import binascii
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


class MessageTypes:
    """
    Message types.
    """
    COMMAND_MESSAGE = 0x10
    RESPONSE_MESSAGE = 0x20


class MessageFactory:
    @staticmethod
    def getMessage(self, data):
        """This message returns an API message from the given data array"""
        # Get the command type
        command_type = self._getCommandType(data)

        message = None
        if command_type == CommandTypes.GET_SERVO_POS:
            message = GetServoPositionMessage().fromPacket(data)
        elif command_type == CommandTypes.GET_ALL_SERVO_POS:
            pass
        elif command_type == CommandTypes.GET_BOARD_STATUS:
            message = GetBoardStatusMessage().fromPacket(data)

        return message

    def _getCommandType(self, data):
        if data is None or len(data < 2):
            return None

        return struct.unpack_from("H", data, 2)


class BaseMessage:
    """
    Base class for an command message packet. An command message is used for
    the communication with the flight controller over a serial connection.
    """

    # Constants
    START_BYTE = 0x8D
    STOP_BYTE = 0x7E

    def __init__(self, commandType):
        self.commandType = commandType
        self.messageType = MessageTypes.COMMAND_MESSAGE

    def getPacket(self):
        """
        This method returns a byte array which can be send via a serial connection.
        Every class that inherets this class must override this method.
        """
        pass

    @staticmethod
    def fromPacket(self, package):
        """
        Creates a new message object from the given data array.
        Every class that inherets this class must override this method
        """
        pass

    def _encodePackage(self, data):
        """
        Returns the encoded message as a byte array.
        
        Params:
        data: An array of dictionaries which contains the arguments and their corresponding lengths
              Format = [{"value": value, "format": format character}]
        """
        package = bytearray()

        # Start byte
        package.extend(struct.pack("B", self.START_BYTE))
        # Message type
        package.extend(struct.pack("B", self.self.messageType))
        # Command type
        package.extend(struct.pack("H", self.commandType))
        # Encode the data
        package = self._packData(package, data)
        # CRC
        package.extend(struct.pack("B", self._calcCrc(package, data)))
        # Stop Byte
        package.extend(struct.pack("B", self.STOP_BYTE))

        return package

    def _decodePackage(self, data):
        pass

    def _packData(self, package, data):
        """Encodes the data and returns the package with the encoded data."""
        if len(data) < 1:
            return

        # Encode all arguments
        for argument in data:
            format = argument["format"]
            package.extend(struct.pack(argument["format"], argument["value"]))

        return package

    def _calcCrc(self, package, data):
        """Calculates the CRC8 for the given package and returns it."""
        msg = bytearray()

        msg.extend(struct.pack("B", self.START_BYTE))
        msg.extend(struct.pack("B", self.self.messageType))
        msg.extend(struct.pack("H", self.commandType))
        msg = self._packData(msg, data)

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
    def fromPacket(self, package):
        # Get the command type
        command_type = struct.unpack_from("H", package, 2)

        if command_type[0] is not CommandTypes.GET_SERVO_POS:
            return None

        # Parse the data
        servo_nr = struct.unpack_from("B", package, 5)
        position = struct.unpack_from("H", package, 6)

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
        self.accelerationX = 0
        self.accelerationY = 0
        self.accelerationZ = 0
        self.gyroX = 0
        self.gyroY = 0
        self.gyroZ = 0

    def getPacket(self):
        return self._encodePackage([])

    @staticmethod
    def fromPacket(self, package):
        # Get the command type
        command_type = struct.unpack_from("H", package, 2)

        if command_type[0] is not CommandTypes.GET_SERVO_POS:
            return None

        message = GetBoardStatusMessage()
        message.messageType = MessageTypes.RESPONSE_MESSAGE

        # Parse the data
        message.outputChannel1 = struct.unpack_from("H", package, 5)
        message.outputChannel2 = struct.unpack_from("H", package, 7)
        message.outputChannel3 = struct.unpack_from("H", package, 9)
        message.outputChannel4 = struct.unpack_from("H", package, 11)
        message.outputChannel5 = struct.unpack_from("H", package, 13)
        message.outputChannel6 = struct.unpack_from("H", package, 15)
        message.inputChannel1 = struct.unpack_from("H", package, 17)
        message.inputChannel2 = struct.unpack_from("H", package, 19)
        message.inputChannel3 = struct.unpack_from("H", package, 21)
        message.inputChannel4 = struct.unpack_from("H", package, 23)
        message.inputChannel5 = struct.unpack_from("H", package, 25)
        message.inputChannel6 = struct.unpack_from("H", package, 27)
        message.accelerationX = struct.unpack_from("H", package, 29)
        message.accelerationY = struct.unpack_from("H", package, 31)
        message.accelerationZ = struct.unpack_from("H", package, 33)
        message.gyroX = struct.unpack_from("H", package, 35)
        message.gyroY = struct.unpack_from("H", package, 37)
        message.gyroZ = struct.unpack_from("H", package, 39)

        return message