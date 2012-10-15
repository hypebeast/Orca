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


import time
import threading
import binascii

from PyQt4.QtCore import QObject, pyqtSignal

#try:
#    from PyQt4 import pyqtSignal
#except ImportError:
#    print "No PyQt found!"
#    import sys
#    sys.exit(2)

from SerialConnection import SerialConnection
from ApiCommands import CommandTypes, ServoPositionMessage, GetBoardStatusMessage
from BoardStatus import BoardStatus
from logger import Logger
from utils import get_all_from_queue 


class ControllerManager(QObject):
	"""
	This class manages the communication with the controller, e.g. the
	connection, status updates, etc.
	"""

	# Update interval in ms
	UPDATE_INTERVAL = 1000

	# This signals is emitted when the board status was updated
	board_status_updated = pyqtSignal()

	def __init__(self):
		QObject.__init__(self)

		# Serial connection
		self.serial = SerialConnection()
		self.serial.set_port(0)
		self.serial.message_received.connect(self._onMessageReceived)

        # Board status object
		self.boardStatus = BoardStatus()

        # Status reader
		self.statusReaderThread = None
		self.statusReaderIsAlive = False
		self.statusReaderUpdateInterval = 1.0 # Update interval in seconds

        # Status queries
		self.status_queries = [
								{"command": CommandTypes.GET_ALL_SERVO_POS, "args": []}
							  ]

		# Indicates if new data from the board was received
		#self.hasNewData = False

		self._logger = Logger()

	def connect(self, port=0):
		"""Connects to the flight controller"""
		self.serial.connect()

		# Start reading the board status
		self._startStatusReader()

	def disconnect(self):
		"""Disconnects from the flight controller"""
		self.serial.disconnect()

		# Stop reading the board status
		self._stopStatusReader()

	def set_serial_port(self, port):
		self.serial.set_port(port)

	def connected(self):
		return self.serial.connected

	def setUpdateInterval(seld, interval):
		pass

	def _startStatusReader(self):
		"""
		Starts the status readers.
		"""
		# Already started?
		if self.statusReaderIsAlive:
			return

		# Create thread and start reading the board status
		self.statusReaderIsAlive = True
		self.statusReaderThread = threading.Thread(target=self._statusReader)
		self.statusReaderThread.setDaemon = True
		self.statusReaderThread.start()

	def _stopStatusReader(self):
		"""
		Stops the status reader.
		"""
		if not self.statusReaderIsAlive:
			return

		self.statusReaderAlive = False
		self.statusReaderThread.join()

	def _statusReader(self):
		"""
		Sends status requests to the flight controller board.
		"""
		while self.statusReaderIsAlive:
			self._logger.debug("Sending new status query")
			message = GetBoardStatusMessage()
			self.serial.writeMessage(message)

			time.sleep(self.statusReaderUpdateInterval)

	def _onMessageReceived(self):
		"""
		Callback method which is called when a new messages was received from
		the serial connection.
		"""
		messages = list(get_all_from_queue(self.serial.messageQueue))
		if len(messages) > 0:
			self._logger.debug("New status response received")
			for message in messages:
				self._updateStatus(message[0], message[1])

	def _updateStatus(self, message, timestamp):
		"""
		Updates the BoardStatus object with the received data from the serial
		connection.
		"""
		self.boardStatus.updateDataFromMessage(message, timestamp)

		self.boardStatus.printStatus()

		# Emit signal
		self.board_status_updated.emit()

	###########################################
	## Output channels (servo, engine) methods
	###########################################

	def setServoPos(self, servo_nr=1, pos=0):
		"""Moves the specified servo to the given position."""
		message = ServoPositionMessage(servo_nr, pos)
		self.serial.writeMessage(message)

	def getServoPos(self, servo_nr=1):
		pass