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

import serial
import threading


class CommandType:
    TEST = "test"
    LEDSON = "ledson"
    LEDSOFF = "ledsoff"
    SETSERVOPOS = "setservopos"


class CommandMessage:
    def __init__(self, commandType, arguments=[]):
        self.commandType = commandType
        self.data = arguments

    def addArgument(self, argument):
        self.data.append(argument)

    def getMessage(self):
        message = self.commandType
        for arg in self.data:
            message = message + " " + arg

        message = message + '\r'
        return message


class SerialAPI:
    def __init__(self, port="0", baudrate=9600):
        self.serial_connection = None
        self.port = port
        self.baudrate = baudrate
        self.connected = False

        self.receiver_thread = None

    def start_reader(self):
        pass

    def stop_reader(self):
        pass
    
    def connect(self):
        if self.serial_connection is not None:
            self.serial_connection.close()
            self.serial_connection = None

        self.serial_connection = serial.Serial(self.port)
        self.connected = True

    def disconnect(self):
        if self.serial_connection is not None:
            self.serial_connection.close()
            self.serial_connection = None
            self.connected = False

    def writeCommand(self, command):
        if self.serial_connection is None or not self.connected:
            return

        self.serial_connection.write(command.getMessage())

    def scan(self):
        pass

    def readStatus(self):
        pass
