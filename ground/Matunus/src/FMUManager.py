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

try:
    from PyQt4.QtCore import pyqtSignal, QObject
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)

from SerialConnection import SerialConnection
from ApiCommands import CommandTypes, ServoPositionMessage, GetBoardStatusMessage
from ApiCommands import GetBoardSettingsMessage, SetRollPIDCoefficientsMessage
from ApiCommands import SetRollKalmanConstantsMessage, SaveSettingsMessage
from BoardStatus import BoardStatus
from BoardSettings import BoardSettings
from logger import Logger
from utils import get_all_from_queue 


# Global firmware settings object
fmuManager = None

class _FMUManager(QObject):
	"""
	This class manages the communication with the controller, e.g. the
	connection, status updates, etc.
	"""

	# Update interval in ms
	UPDATE_INTERVAL = 1000

	# This signals is emitted when the board status was updated
	board_status_updated = pyqtSignal()

	# This signal is emitted when the board settings are updated
	board_settings_updated = pyqtSignal()

	# This signal is emitted when a new data object message was received
	data_object_received = pyqtSignal()

	def __init__(self):
		QObject.__init__(self)

		# Serial connection
		self.serial = SerialConnection()
		self.serial.set_port(0)
		self.serial.message_received.connect(self._onMessageReceived)

        # Board status object
		self.boardStatus = BoardStatus()

		# Board settings object
		self.boardSettings = BoardSettings()

        # Status reader
		self.statusReaderThread = None
		self.statusReaderIsAlive = False
		self.statusReaderUpdateInterval = 0.25 # Update interval in seconds

		# Indicates if new data from the board was received
		#self.hasNewData = False

		self._logger = Logger()

	def connect(self, port=0):
		"""Connects to the flight controller"""
		self.serial.connect()

		# Start reading the board status
		self._startStatusReader()

		# Read once the board settings
		self.updateBoardSettings()

	def disconnect(self):
		"""Disconnects from the flight controller"""
		# Stop the status reader
		self._stopStatusReader()

		# Disconnects
		self.serial.disconnect()

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

		self.statusReaderIsAlive = False
		#time.sleep(self.statusReaderUpdateInterval)
		self.statusReaderThread.join()

	def _statusReader(self):
		"""
		Sends status requests to the flight controller board.
		"""
		while self.statusReaderIsAlive:
			#self._logger.debug("Sending new status query")
			message = GetBoardStatusMessage()
			self.serial.writeMessage(message)

			#self.updateBoardSettings()

			time.sleep(self.statusReaderUpdateInterval)

	def _onMessageReceived(self):
		"""
		Callback method which is called when a new messages was received by
		the serial connection.
		"""
		try:
			messages = list(get_all_from_queue(self.serial.messageQueue))
			#message = self.serial.messageQueue.get(False)
			#if message[0] is None:
			#	return
			#if message[0].commandType == CommandTypes.GET_BOARD_STATUS:
			#	self._updateStatus(message[0], message[1])
			#elif message[0].commandType == CommandTypes.GET_BOARD_SETTINGS:
			#	self._onBoardSettingsUpdated(message[0], message[1])
			if len(messages) > 0:
				#self._logger.debug("New status response received")
				for message in messages:
					if message[0] is None:
						continue
					if message[0].commandType == CommandTypes.GET_BOARD_STATUS:
						self._updateStatus(message[0], message[1])
					elif message[0].commandType == CommandTypes.GET_BOARD_SETTINGS:
						self._onBoardSettingsUpdated(message[0], message[1])
					elif message[0].commandType == CommandTypes.DEBUG_INT_VALUES:
						self.boardStatus.updateFromMessage(message[0], message[1])
						# Emit signal
						self.board_status_updated.emit()
					elif message[0].commandType == CommandTypes.DEBUG_FLOAT_VALUES:
						self.boardStatus.updateFromMessage(message[0], message[1])
						# Emit signal
						self.board_status_updated.emit()
					elif message[0].commandType == CommandTypes.DEBUG_STRING_MESSAGE:
						self.boardStatus.updateFromMessage(message[0], message[1])
						# Emit signal
						self.board_status_updated.emit()
					else:
						self.data_object_received.emit()
		except StopIteration:
			return

	def _updateStatus(self, message, timestamp):
		"""
		Updates the board status with the received data from the serial
		connection.
		"""
		self.boardStatus.updateFromMessage(message, timestamp)
		#self.boardStatus.printStatus()
		# Emit signal
		self.board_status_updated.emit()

	def _onBoardSettingsUpdated(self, message, timestamp):
		"""Called when an board settings (0x0020) message was received. The board
		settings will be updated with the received message."""
		self.boardSettings.updateFromMessage(message, timestamp)
		# Emit signal
		self.board_settings_updated.emit()

	###########################################
	## Output channels (servo, engine) methods
	###########################################

	def setServoPos(self, servo_nr=1, pos=0):
		"""Moves the specified servo to the given position."""
		message = ServoPositionMessage(servo_nr, pos)
		self.serial.writeMessage(message)

	def getServoPos(self, servo_nr=1):
		pass

	###########################################
	## Board settings methods
	###########################################	
	
	def updateBoardSettings(self):
		"""Sends an update board settings message."""
		message = GetBoardSettingsMessage()
		self.serial.writeMessage(message)

	def saveBoardSettings(self):
		"""Saves all firmware settings to the flash memory."""
		message = SaveSettingsMessage()
		self.serial.writeMessage(message)

	def setRollPIDCoefficients(self, p_fac, i_fac, d_fac, i_limit):
		message = SetRollPIDCoefficientsMessage(p_fac, i_fac, d_fac, i_limit)
		self.serial.writeMessage(message)

	def setRollKalmanConstants(self, q_angle, q_gyro, r_angle):
		message = SetRollKalmanConstantsMessage(q_angle, q_gyro, r_angle)
		self.serial.writeMessage(message)


def FMUManager():
	"""Singleton instance for the FMU Manager"""
	global fmuManager
	if not fmuManager:
		fmuManager = _FMUManager()
	return fmuManager