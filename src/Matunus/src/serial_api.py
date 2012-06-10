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

import serial
import threading
import sys

from logger import Logger


NEWLINE_CONVERISON_MAP = ('\n', '\r', '\r\n')
LF_MODES = ('LF', 'CR', 'CR/LF')
NEWLINECHARACTER = '\r\n'

DEFAULT_PORT = 0
DEFAULT_BAUDRATE = 9600
DEFAULT_PARITY = serial.PARITY_NONE
DEFAULT_BYTESIZE = serial.EIGHTBITS
DEFAULT_STOPBITS = serial.STOPBITS_ONE
DEFAULT_XONXOFF = True
DEFAULT_RTS = None
DEFUALT_DTR = None


if sys.version_info >= (3, 0):
    def character(b):
        return b.decode('latin1')
else:
    def character(b):
        return b


class SerialError(Exception):
    def __init__(self, msg):
        Exception.__init__(self)
        self.msg = msg

    def __str__(self):
        return self.msg


class CommandType:
    LEDSON = "ledson"
    LEDSOFF = "ledsoff"
    SETSERVOPOS = "SERVO POS"
    READSERVOPOS = "SERVO GETPOS"


class ResponseStatus:
    IDLE = 0
    RECEIVING = 1
    CR_RECEIVED = 2
    FINSIHED = 3
    RESPONSE_FINISHED = 4


class CommandMessage:
    def __init__(self, commandType):
        self.commandType = commandType
        self.data = []

    def addArgument(self, argument):
        self.data.append(argument)

    def getMessage(self):
        message = self.commandType
        for arg in self.data:
            message = message + ' ' + arg

        return message + NEWLINECHARACTER


class SerialAPI:
    def __init__(self, port=DEFAULT_PORT,
                 baudrate=DEFAULT_BAUDRATE,
                 parity=DEFAULT_PARITY,
                 bytesize=DEFAULT_BYTESIZE,
                 stopbits=DEFAULT_STOPBITS,
                 xonxoff=DEFAULT_XONXOFF):
        self.serial_connection = None
        self.port = port
        self.baudrate = baudrate
        self.parity = parity
        self.bytesize = bytesize
        self.stopbits = stopbits
        self.xonxoff = xonxoff
        self.connected = False

        self.receiver_thread = None
        self.reader_alive = False
        self.newResponse = False
        self.responseStatus = ResponseStatus.IDLE
        self.responseBuffer = []

        self.status_queries = [{"query": "SERVO GETPOS", "args": ["1"], "data": 0}]

        self._logger = Logger()

    def start_reader(self):
        """Start reader thread"""
        self.reader_alive = True
        self.receiver_thread = threading.Thread(target = self.reader)
        self.receiver_thread.setDaemon(True)
        self.receiver_thread.start()

    def stop_reader(self):
        """Stop reader thread only, wait for clean exit of thread"""
        self.reader_alive = False
        self.receiver_thread.join()

    def connect(self):
        if self.serial_connection is not None:
            self.serial_connection.close()
            self.serial_connection = None

        try:
            self.serial_connection = serial.serial_for_url(self.port,
                                            self.baudrate,
                                            parity=self.parity,
                                            bytesize=self.bytesize,
                                            xonxoff=self.xonxoff,
                                            rtscts=False,
                                            dsrdtr=False,
                                            timeout=1)
            #self.serial_connection = serial.Serial(self.port)
            self.connected = True
        except serial.SerialException as ex:
            #errors = [ error.msg for error in ex.strerror ]
            #errors = ";".join(errors)
            raise SerialError("Could not connect to COM Port %s: %s" % (self.port, ex.strerror))

        # Start reader thread
        self.start_reader()

    def disconnect(self):
        if self.serial_connection is not None:
            # Stop reader thread
            self.stop_reader()

            self.serial_connection.close()
            self.serial_connection = None
            self.connected = False

    def set_port(self, port):
        if port is None:
            return

        self.port = port

    def set_baudrate(self, baudrate):
        if baudrate is None or baudrate < 0:
            return

        self.baudrate = baudrate

    def writeCommand(self, command):
        if self.serial_connection is None or not self.connected:
            return

        self.serial_connection.write(command.getMessage())
        self.serial_connection.flush()

    def scan(self):
        pass

    def reader(self):
        try:
            while self.connected and self.reader_alive:
                #data = character(self.serial_connection.read(1))
                data = self.serial_connection.read(1)
                print "Data: " + data

                if data != '':
                    print "Received data: " + data

                    if self.responseStatus == ResponseStatus.IDLE:
                        self.responseStatus = ResponseStatus.RECEIVING
                        self.responseBuffer = []
                        self.responseBuffer.append(data)
                    elif self.responseStatus == ResponseStatus.RECEIVING:
                        if data == '\r':
                            self.responseStatus = ResponseStatus.CR_RECEIVED
                        else:
                            self.responseBuffer.append(data)
                    elif self.responseStatus == ResponseStatus.CR_RECEIVED:
                        if data == '\n':
                            self.responseStatus = ResponseStatus.RESPONSE_FINISHED
                    elif self.responseStatus == ResponseStatus.RESPONSE_FINISHED:
                        self.responseStatus = ResponseStatus.IDLE
                        self.processResponse(data)

        except serial.SerialException, e:
            self.reader_alive = False
            raise

    def processResponse(self, data):
        pass

    def readStatus(self):
        """
        This method reads the current status from the controller
        """
        for i in range(0, len(self.status_queries)):
            query = self.status_queries[i]

            command = CommandMessage(query["query"])
            for arg in query["args"]:
                command.addArgument(arg)

            #self._logger.debug("Execute query: " + command.getMessage())
            print "Execute query: " + command.getMessage()
            self.writeCommand(command)

            #self._logger.info("Query: %s Data: %s" % (query["query"], query["data"]))
