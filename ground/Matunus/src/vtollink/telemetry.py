# Copyright (C) 2013 Sebastian Ruml <sebastian.ruml@gmail.com>
#
# This file is part of the Matunus project (part of the Orcacopter project)
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

try:
    from PyQt4.QtCore import pyqtSignal, QObject
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)

from ..logger import Logger
from vtollink import VTOLLink
from ..vtolobjects import vtolobject


DEFAULT_PORT = 0
DEFAULT_BAUDRATE = 57600
DEFAULT_PARITY = serial.PARITY_NONE
DEFAULT_BYTESIZE = serial.EIGHTBITS
DEFAULT_STOPBITS = serial.STOPBITS_ONE
DEFAULT_XONXOFF = False
DEFAULT_RTS = None
DEFUALT_DTR = None


# Global telemetry object
telemetry_object = None


class TelemetryException(Exception):
    def __init__(self, msg):
        Exception.__init__(self)
        self.msg = msg


class _Telemetry(QObject):
    """Sends and retreives bytes over a serial connection."""

    # This signal is emitted if a new VTOL object is received
    vtol_object_received = pyqtSignal(vtolobject.VTOLObject)

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

         # Logger
        self._logger = Logger()

        # VTOL Link
        self._vtolLink = VTOLLink()
        self._vtolLink.vtol_object_received.connect(self._onObjectRecveived)

        # Create RX task
        self.rx_thread = None
        self.rx_thread_alive = False
        self.rx_thread_interval = 0.01  # Update interval in seconds

    def connect(self):
        """"Connects to the FMU."""
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
            self.connected = True
        except serial.SerialException as ex:
            raise TelemetryException("Could not connect to COM Port %s: %s" % (self.port, ex.strerror))

        # Start RX task
        self._start_rx_thread()

    def disconnect(self):
        """"Disconnects from the FMU."""
        if self.serial_connection is not None:
            # Stop RX task
            self._stop_rx_thread()

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

        self.rx_thread_interval = interval

    def transmitData(self, data):
        """Transmit the given data. The data parameter needs to be an byte array."""
        try:
            self.serial_connection.write(data)
        except serial.SerialException as ex:
            self._logger(ex.value)

    def sendObject(self, obj, req, ackReq):
        self._sendObject(obj, 1, req, ackReq, 1000)

    def _sendObject(self, obj, instId, req, ackReq, timeoutMs):
        self._vtolLink.sendObject(obj, self.transmitData, instId, req, ackReq, timeoutMs)

    def _start_rx_thread(self):
        """"Starts the RX task"""
        self.rx_thread_alive = True
        self.rx_thread = threading.Thread(target=self._rx_task)
        self.rx_thread.setDaemon(True)
        self.rx_thread.start()

    def _stop_rx_thread(self):
        """"Stops the RX task, wait for clean exit of thread."""
        self.rx_thread_alive = False
        self.rx_thread.join()

    def _rx_task(self):
        """The RX task reads data from the serial connection."""
        try:
            while self.connected and self.rx_thread_alive:
                # Read one byte, blocking
                data = self.serial_connection.read(1)
                # Look if there is more
                n = self.serial_connection.inWaiting()
                if n:
                    data = data + self.serial_connection.read(n)

                if data is not None and len(data) > 0:
                    for rxByte in data:
                        self._vtolLink.processInputStream(rxByte)

                # Wait for some time
                #time.sleep(self.reader_interval)
        except serial.SerialException as ex:
            self._logger(ex.value)

    def _onObjectRecveived(self, obj):
        if not obj:
            return

        # Emit signal that a new VTOL object was received
        self.vtol_object_received.emit(obj)


def Telemetry():
    global telemetry_object
    if telemetry_object is None:
        telemetry_object = _Telemetry()

    return telemetry_object
