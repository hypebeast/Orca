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
    SET_SERVO_POS = 0x01
    READ_SERVO_POS = 0x02


class CommandMessage:
    """
    Base class for an command message packet. An command message is used for
    the communication with the flight controller over a serial connection.
    """

    START_BYTE = 0x8D
    STOP_BYTE = 0x7E
    MESSAGE_COMMAND = 0x10
    MESSAGE_RESPONSE = 0x20

    def __init__(self, commandType):
        self.commandType = commandType
        self.data = []


    def getMessage(self, data):
        """
        Returns the encoded message as a byte array.
        
        Params:
        data: A dictionary which contains the arguments and their corresponding lengths
        """
        package = bytearray()

        # Calculate the CRC-8 checksum
        #crc_msg = struct.pack('BBhBBh', self.START_BYTE, 0x10, self.commandType, 0x03, self.servo_nr, self.position)
        #crc = crc8()
        #c = crc.crc(crc_msg)
        c = 0x11
        print "CRC: " + str(c)

        # Start byte
        package.extend(struct.pack("B", self.START_BYTE))
        # Message type
        package.extend(struct.pack("B", self.MESSAGE_COMMAND))
        # Command type
        package.extend(struct.pack("H", self.commandType))
        # Data length
        package.extend(struct.pack("B", 3))
        # Data: Servo number
        package.extend(struct.pack("B", self.servo_nr))
        # Data: Position
        package.extend(struct.pack("H", self.position))
        # CRC
        package.extend(struct.pack("B", c))
        # Stop Byte
        package.extend(struct.pack("B", self.STOP_BYTE))

    def fromMessage(self, data):
        """
        Creates a new message object from the given data.
        """
        pass


class ServoPositionCommand(CommandMessage):
    """
    This command sets a new position to the specified servo.
    """
    def __init__(self, servo_nr = 1, position = 0):
        CommandMessage.__init__(self, CommandType.SET_SERVO_POS)
        self.servo_nr = servo_nr
        self.position = position

    def getMessage(self):
        package = bytearray()

        # Calculate the CRC-8 checksum
        #crc_msg = struct.pack('BBhBBh', self.START_BYTE, 0x10, self.commandType, 0x03, self.servo_nr, self.position)
        #crc = crc8()
        #c = crc.crc(crc_msg)
        c = 0x11
        print "CRC: " + str(c)

        # Start byte
        package.extend(struct.pack("B", self.START_BYTE))
        # Message type
        package.extend(struct.pack("B", self.MESSAGE_COMMAND))
        # Command type
        package.extend(struct.pack("H", self.commandType))
        # Data length
        package.extend(struct.pack("B", 3))
        # Data: Servo number
        package.extend(struct.pack("B", self.servo_nr))
        # Data: Position
        package.extend(struct.pack("H", self.position))
        # CRC
        package.extend(struct.pack("B", c))
        # Stop Byte
        package.extend(struct.pack("B", self.STOP_BYTE))

        print binascii.hexlify(package)

        #for i in package:
            #print chr(i)

        message = ''.join(chr(b) for b in package)
        
        return message
        #return package

    def fromMessage(self, data):
        pass