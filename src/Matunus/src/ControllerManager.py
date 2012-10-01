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


from SerialConnection import SerialConnection
from ApiCommands import CommandTypes
from BoardStatus import BoardStatus
from Observer import Observable
from logger import Logger


class ControllerManager(Observable):
	"""
	This class manages the communication with the controller, e.g. the
	connection, status updates, etc.
	"""

	# Update interval in ms
	UPDATE_INTERVAL = 1000

	def __init__(self):
		# Serial connection
		self.serial = SerialConnection()
		self.serial.set_port(0)

        # Board status data
		self.boardStatus = BoardStatus()

        # Status reader
		self.statusReaderThread = None
		self.statusReaderIsAlive = False

        # Status queries
		self.status_queries = [
								{"command": CommandTypes.READ_ALL_SERVO_POS, "args": []}
							  ]

	def connect(self, port=0):
		"""Connects to the flight controller"""
		self.serial.connect()

	def disconnect(self):
		"""Disconnects from the flight controller"""
		self.serial.disconnect()

	def set_serial_port(self, port):
		self.serial.set_port(port)

	def connected(self):
		return self.serial.connected

	def _startStatusReader(self):
		if self.statusReaderIsAlive:
			return

		self.statusReaderThread = Thread(target=self._updateStatus)
		self.statusReaderThread.setDaemon = True
		self.statusReaderThread.start()
		self.statusReaderIsAlive = True

	def _stopStatusReader(self):
		if not self.statusReaderIsAlive:
			return

		self.statusReaderAlive = False
		self.serialReaderThread.join()

	def _updateStatus(self):
		while self.statusReaderIsAlive:
			pass

	############################
	## Servo methods
	############################

	def setServoPos(self, servo_nr=1, pos=0):
		message = ServoPositionCommand(servo_nr, pos)
		self.serial.writeCommand(message)