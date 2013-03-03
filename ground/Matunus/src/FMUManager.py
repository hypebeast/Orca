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
    from PyQt4.QtCore import pyqtSignal, QObject
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)

from vtollink import telemetry
from vtolobjects import vtolobject
from logger import Logger


# Global flight management unit management object
fmuManager = None


class _FMUManager(QObject):
    """
    This class manages the communication with the controller, e.g. the
    connection, status updates, etc.
    """

    # This signal is emitted when a new vtol object was received
    vtol_object_received = pyqtSignal(vtolobject.VTOLObject)

    def __init__(self):
        QObject.__init__(self)

        # Telemetry
        self._telemetry = telemetry.Telemetry()
        self._telemetry.set_port(0)
        self._telemetry.vtol_object_received.connect(self._onObjectReceived)

        self._logger = Logger()

    def connect(self, port=0):
        """Connects to the flight management unit."""
        self._telemetry.connect()

    def disconnect(self):
        """Disconnects from the flight controller"""
        self._telemetry.disconnect()

    def set_serial_port(self, port):
        self.serial.set_port(port)

    def connected(self):
        return self._telemetry.connected

    def _onObjectReceived(self, obj):
        if not obj:
            return

        # Emit signal
        self.vtol_object_received.emit(obj)

    ###########################################
    ## VTOL object methods
    ###########################################

    def sendObject(self, obj, req=False, ackReq=False):
        """Sends the specified VTOL object through the serial line."""
        if not obj:
            return

        self._telemetry.sendObject(obj, req, ackReq)


def FMUManager():
    """Singleton instance for the FMU Manager"""
    global fmuManager
    if not fmuManager:
        fmuManager = _FMUManager()
    return fmuManager
