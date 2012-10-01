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


from crc8 import crc8
from logger import Logger


class CommandTypes:
    """
    This class defines the API command types.
    """
    SET_SERVO_POS = 0x0001
    READ_SERVO_POS = 0x0002
    READ_ALL_SERVO_POS = 0x00003


class BaseMessage:
    """
    Base class for an command message packet. An command message is used for
    the communication with the flight controller over a serial connection.
    """

    # Constants
    START_BYTE = 0x8D
    STOP_BYTE = 0x7E
    COMMAND_MESSAGE = 0x10
    RESPONSE_MESSAGE = 0x20

    MessageType = COMMAND_MESSAGE
    CommandType = 0

    def __init__(self, commandType):
        self.commandType = commandType

    def getMessage(self):
        """
        This method returns a byte array which can be send via a serial connection.
        Every class that inherets this class must override this method.
        """
        pass

    def fromMessage(self, data):
        """
        Creates a new message object from the given data.
        Every class that inherets this class must override this method
        """
        pass

    def _encodeMessage(self, data):
        """
        Returns the encoded message as a byte array.
        
        Params:
        data: An array of dictionaries which contains the arguments and their corresponding lengths
              Format = [{"value": value, "format": format char}]
        """
        package = bytearray()

        # Start byte
        package.extend(struct.pack("B", self.START_BYTE))
        # Message type
        package.extend(struct.pack("B", self.MessageType))
        # Command type
        package.extend(struct.pack("H", self.commandType))
        # Encode the data
        package = self._packData(package, data)
        # CRC
        package.extend(struct.pack("B", _calcCrc(package, data)))
        # Stop Byte
        package.extend(struct.pack("B", self.STOP_BYTE))

        return package

    def _decodeMessage(self, data):
        pass

    def _packData(self, package, data):
        """Encodes the data and returns the extended package."""
        # Encode all arguments
        for argument in data:
            format = argument["format"]
            package.extend(struct.pack(argument["format"], argument["value"]))

        return package

    def _calcCrc(self, package, data):
        """Calculates the CRC8 for the given package and returns it."""
        msg = bytearray()

        msg.extend(struct.pack("B", self.START_BYTE))
        mag.extend(struct.pack("B", self.MessageType))
        msg.extend(struct.pack("H", self.commandType))
        msg = self._packData(msg, data)

        crc = crc8()
        return crc.crc(msg)



class ServoPositionCommand(BaseMessage):
    """
    This command sets a new position to the specified servo.

    Command Type = 0x0001
    """
    def __init__(self, servo_nr = 1, position = 0):
        BaseMessage.__init__(self, CommandType.SET_SERVO_POS)
        self.MessageType = COMMAND_MESSAGE
        
        self.servo_nr = servo_nr
        self.position = position

    def getMessage(self):
       data = []
       data.append({"value": self.servo_nr, "format": "B"})
       data.append({"value": self.position, "format": "H"})

       return self._encodeMessage(data)

    def fromMessage(self, data):
        pass