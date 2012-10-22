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
import Queue
import time
import array

from PyQt4.QtCore import QObject, pyqtSignal

#try:
#    from PyQt4 import pyqtSignal
#except ImportError:
#    print "No PyQt found!"
#    import sys
#    sys.exit(2)

from crc8 import crc8
from logger import Logger
from ApiCommands import MessageFactory


NEWLINE_CONVERISON_MAP = ('\n', '\r', '\r\n')
LF_MODES = ('LF', 'CR', 'CR/LF')
NEWLINECHARACTER = '\r\n'

DEFAULT_PORT = 0
DEFAULT_BAUDRATE = 57600
DEFAULT_PARITY = serial.PARITY_NONE
DEFAULT_BYTESIZE = serial.EIGHTBITS
DEFAULT_STOPBITS = serial.STOPBITS_ONE
DEFAULT_XONXOFF = False
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


class ResponseStatus:
    IDLE = 0
    START_RECEIVED = 1


class SerialConnection(QObject):
    """
    This class handles the serial connection with the flight controller.
    """

    START_BYTE = 0x8D
    STOP_BYTE = 0x7E

    # This signal is emitted if a new message was received
    message_received = pyqtSignal()

    def __init__(self, port=DEFAULT_PORT,
                 baudrate=DEFAULT_BAUDRATE,
                 parity=DEFAULT_PARITY,
                 bytesize=DEFAULT_BYTESIZE,
                 stopbits=DEFAULT_STOPBITS,
                 xonxoff=DEFAULT_XONXOFF):
        QObject.__init__(self)

        self.serial_connection = None
        self.port = port
        self.baudrate = baudrate
        self.parity = parity
        self.bytesize = bytesize
        self.stopbits = stopbits
        self.xonxoff = xonxoff
        self.connected = False

        # Receiver
        self.receiver_thread = None
        self.reader_alive = False
        self.newResponse = False
        self.responseStatus = ResponseStatus.IDLE
        self.responseBuffer = array.array('c')
        self.reader_interval = 0.01 # Update interval in seconds

        # This queue contains the received messages. The messages are stored as
        # tuples in the following format: (message, timestamp).
        self.messageQueue = Queue.Queue()

        # Logger
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

    def set_update_rate(self, interval):
        if interval <= 0.0:
            return

        self.reader_interval = interval

    def writeMessage(self, command):
        """
        This method writes the given packet to the serial communication line.
        """
        if self.serial_connection is None or not self.connected or command is None:
            return

        #packet = command.getPacket()
        #hexdata = ''.join('%02x' % ord(chr(byte)) for byte in packet)
        #self._logger.debug(hexdata)

        try:
            self.serial_connection.write(command.getPacket())
            #self.serial_connection.flush()
        except:
            pass

    def scan(self):
        pass

    def reader(self):
        """Reads data from the serial line"""
        try:
            while self.connected and self.reader_alive:
                # Read one byte, blocking
                data = self.serial_connection.read(1)
                # Look if there is more
                n = self.serial_connection.inWaiting()
                if n:
                    data = data + self.serial_connection.read(n)

                if data is not None and len(data) > 0:
                    #bytes = ''.join('%02x' % ord(b) for b in data)
                    #self._logger.debug("Received data: " + bytes)
                    for byte in data:
                        # Get an integer value for checking the start or stop byte
                        value = ord(byte)
                        #self._logger.debug("Received data: %02x" % ord(byte))
                        if self.responseStatus == ResponseStatus.IDLE:
                            if value == self.START_BYTE:
                                self.responseStatus = ResponseStatus.START_RECEIVED
                                self.responseBuffer = array.array('c')
                                self.responseBuffer.append(byte)
                        elif self.responseStatus == ResponseStatus.START_RECEIVED:
                            if value == self.STOP_BYTE:
                                self.responseStatus = ResponseStatus.IDLE
                                self.responseBuffer.append(byte)
                                self.processReceivedMessage(self.responseBuffer)
                            elif value == self.START_BYTE:
                                self.responseBuffer.append(byte)
                            else:
                                self.responseBuffer.append(byte)

                # Wait for some time
                #time.sleep(self.reader_interval)
        except serial.SerialException, e:
            self.reader_alive = False

    def processReceivedMessage(self, buffer):
        """
        Processes a received message packet. It's called when a new message
        was received.
        """
        message = MessageFactory.getMessage(buffer)

        timestamp = time.clock()
        self.messageQueue.put((message, timestamp))

        # Emit signal to inform all connected classes
        self.message_received.emit()
