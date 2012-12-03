# Copyright (C) 2012 Sebastian Ruml <sebastian.ruml@gmail.com>
#
# This file is part of the Matunus project (part of the Orca project).
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

import FMUManager
from ApiCommands import CommandTypes
from DataObjectField import DataObjectField, DataFieldTypes
from logger import Logger


class BoardStatus(object):
	"""
	This class holds the current status of the flight controller.
	"""
	def __init__(self):
		self.dataFields = list()
		self._lastUpdate = 0

		# FMU Manager
		#self._fmuManager = FMUManager.FMUManager()
		#self._fmuManager.data_object_received.connect(self._onDataObjectReceived)

		# Logger
		self._logger = Logger()

		# Create data fields
		self._createDataFields()

	def _createDataFields(self):
		"""
		Creates all data fields.
		"""
		self.dataFields.append(DataObjectField("outputChannel1", "ticks", DataFieldTypes.Int))
		self.dataFields.append(DataObjectField("outputChannel2", "ticks", DataFieldTypes.Int))
		self.dataFields.append(DataObjectField("outputChannel3", "ticks", DataFieldTypes.Int))
		self.dataFields.append(DataObjectField("outputChannel4", "ticks", DataFieldTypes.Int))
		self.dataFields.append(DataObjectField("outputChannel5", "ticks", DataFieldTypes.Int))
		self.dataFields.append(DataObjectField("outputChannel6", "ticks", DataFieldTypes.Int))
		self.dataFields.append(DataObjectField("inputChannel1", "ticks", DataFieldTypes.Int))
		self.dataFields.append(DataObjectField("inputChannel2", "ticks", DataFieldTypes.Int))
		self.dataFields.append(DataObjectField("inputChannel3", "ticks", DataFieldTypes.Int))
		self.dataFields.append(DataObjectField("inputChannel4", "ticks", DataFieldTypes.Int))
		self.dataFields.append(DataObjectField("inputChannel5", "ticks", DataFieldTypes.Int))
		self.dataFields.append(DataObjectField("inputChannel6", "ticks", DataFieldTypes.Int))
		self.dataFields.append(DataObjectField("inputChannel7", "ticks", DataFieldTypes.Int))
		self.dataFields.append(DataObjectField("accelerationX", "Grad/s^2", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("accelerationY", "Grad/s^2", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("accelerationZ", "Grad/s^2", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("gyroX", "Grad", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("gyroY", "Grad", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("gyroZ", "Grad", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("kalmanOutputRoll", "Grad", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("kalmanReferenceValueRoll", "Grad", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("setValueRollAngle", "Grad", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("actuatingVariablePidRoll", "Grad", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("debugIntValue1", "", DataFieldTypes.Int))
		self.dataFields.append(DataObjectField("debugIntValue2", "", DataFieldTypes.Int))
		self.dataFields.append(DataObjectField("debugIntValue3", "", DataFieldTypes.Int))
		self.dataFields.append(DataObjectField("debugIntValue4", "", DataFieldTypes.Int))
		self.dataFields.append(DataObjectField("debugIntValue5", "", DataFieldTypes.Int))
		self.dataFields.append(DataObjectField("debugFloatValue1", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("debugFloatValue2", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("debugFloatValue3", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("debugFloatValue4", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("debugFloatValue5", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("debugStringMessage", "", DataFieldTypes.String))

	def setValue(self, name, value):
		"""Updates the specified field with the given value."""
		pass

	def getValue(self, name):
		"""Returns the value of the specified data field."""
		for field in self.dataFields:
			if field.name == name:
				return field.value

		return None

	def updateFromMessage(self, message, timestamp):
		"""
		Updates all data fields from an API message.
		"""
		if message is None:
			return

		if message.commandType == CommandTypes.GET_SERVO_POS:
			pass
		elif message.commandType == CommandTypes.GET_BOARD_STATUS:
			self._lastUpdate = timestamp
			for field in self.dataFields:
				if field.name == "outputChannel1":
					field.value = message.outputChannel1
				elif field.name == "outputChannel2":
					field.value = message.outputChannel2
				elif field.name == "outputChannel3":
					field.value = message.outputChannel3
				elif field.name == "outputChannel4":
					field.value = message.outputChannel4
				elif field.name == "outputChannel5":
					field.value = message.outputChannel5
				elif field.name == "outputChannel6":
					field.value = message.outputChannel6
				elif field.name == "inputChannel1":
					field.value = message.inputChannel1
				elif field.name == "inputChannel2":
					field.value = message.inputChannel2
				elif field.name == "inputChannel3":
					field.value = message.inputChannel3
				elif field.name == "inputChannel4":
					field.value = message.inputChannel4
				elif field.name == "inputChannel5":
					field.value = message.inputChannel5
				elif field.name == "inputChannel6":
					field.value = message.inputChannel6
				elif field.name == "inputChannel7":
					field.value = message.inputChannel7
				elif field.name == "accelerationX":
					field.value = message.accelerationX
				elif field.name == "accelerationY":
					field.value = message.accelerationY
				elif field.name == "accelerationZ":
					field.value = message.accelerationZ
				elif field.name == "gyroX":
					field.value = message.gyroX
				elif field.name == "gyroY":
					field.value = message.gyroY
				elif field.name == "gyroZ":
					field.value = message.gyroZ
				elif field.name == "kalmanOutputRoll":
					field.value = message.kalmanOutputRoll
				elif field.name == "kalmanReferenceValueRoll":
					field.value = message.kalmanReferenceValueRoll
				elif field.name == "setValueRollAngle":
					field.value = message.setValueRollAngle
				elif field.name == "actuatingVariablePidRoll":
					field.value = message.actuatingVariablePidRoll
		elif message.commandType == CommandTypes.DEBUG_INT_VALUES:
			self._lastUpdate = timestamp
			for field in self.dataFields:
				if field.name == "debugIntValue1":
					field.value = message.value1
				elif field.name == "debugIntValue2":
					field.value = message.value2
				elif field.name == "debugIntValue3":
					field.value = message.value3
				elif field.name == "debugIntValue4":
					field.value = message.value4
				elif field.name == "debugIntValue5":
					field.value = message.value5
		elif message.commandType == CommandTypes.DEBUG_FLOAT_VALUES:
			self._lastUpdate = timestamp
			for field in self.dataFields:
				if field.name == "debugFloatValue1":
					field.value = message.value1
				elif field.name == "debugFloatValue2":
					field.value = message.value2
				elif field.name == "debugFloatValue3":
					field.value = message.value3
				elif field.name == "debugFloatValue4":
					field.value = message.value4
				elif field.name == "debugFloatValue5":
					field.value = message.value5
		elif message.commandType == CommandTypes.DEBUG_STRING_MESSAGE:
			self._lastUpdate = timestamp
			for field in self.dataFields:
				if field.name == "debugStringMessage":
					field.value = message.message
					break
		else:
			return

		#self.printStatus()

	def _onDataObjectReceived(self):
		pass

	@property
	def outputChannel1(self):
		return self.getValue('outputChannel1')

	@property
	def outputChannel2(self):
		return self.getValue('outputChannel2')

	@property
	def outputChannel3(self):
		return self.getValue('outputChannel3')

	@property
	def outputChannel4(self):
		return self.getValue('outputChannel4')

	@property
	def outputChannel5(self):
		return self.getValue('outputChannel5')

	@property
	def outputChannel6(self):
		return self.getValue('outputChannel6')

	@property
	def inputChannel1(self):
		return self.getValue('inputChannel1')

	@property
	def inputChannel2(self):
		return self.getValue('inputChannel2')

	@property
	def inputChannel3(self):
		return self.getValue('inputChannel3')

	@property
	def inputChannel4(self):
		return self.getValue('inputChannel4')

	@property
	def inputChannel5(self):
		return self.getValue('inputChannel5')

	@property
	def inputChannel6(self):
		return self.getValue('inputChannel6')

	@property
	def inputChannel7(self):
		return self.getValue('inputChannel7')

	@property
	def accelerationX(self):
		return self.getValue('accelerationX')

	@property
	def accelerationY(self):
		return self.getValue('accelerationY')

	@property
	def accelerationZ(self):
		return self.getValue('accelerationZ')

	@property
	def gyroX(self):
		return self.getValue('gyroX')

	@property
	def gyroY(self):
		return self.getValue('gyroY')

	@property
	def gyroZ(self):
		return self.getValue('gyroZ')

	@property
	def kalmanOutputRoll(self):
		return self.getValue('kalmanOutputRoll')

	@property
	def kalmanReferenceValueRoll(self):
		return self.getValue('kalmanReferenceValueRoll')

	@property
	def setValueRollAngle(self):
		return self.getValue('setValueRollAngle')

	@property
	def actuatingVariablePidRoll(self):
		return self.getValue('actuatingVariablePidRoll')

	@property
	def debugIntValue1(self):
		return self.getValue("debugIntValue1")

	@property
	def debugIntValue2(self):
		return self.getValue("debugIntValue2")

	@property
	def debugIntValue3(self):
		return self.getValue("debugIntValue3")

	@property
	def debugIntValue4(self):
		return self.getValue("debugIntValue4")

	@property
	def debugIntValue5(self):
		return self.getValue("debugIntValue5")

	@property
	def debugFloatValue1(self):
		return self.getValue("debugFloatValue1")

	@property
	def debugFloatValue2(self):
		return self.getValue("debugFloatValue2")

	@property
	def debugFloatValue3(self):
		return self.getValue("debugFloatValue3")

	@property
	def debugFloatValue4(self):
		return self.getValue("debugFloatValue4")

	@property
	def debugFloatValue5(self):
		return self.getValue("debugFloatValue5")

	@property
	def debugStringMessage(self):
		return self.getValue("debugStringMessage")

	@property
	def lastUpdate(self):
		return self._lastUpdate

	def printStatus(self):
		"""
		Prints the current status to stdout.
		"""
		#self._logger.info("Board Status")
		#self._logger.info("--------------------------")
		# self._logger.info("Output Channel 1: %s" % self.outputChannel1)
		# self._logger.info("Output Channel 2: %s" % self.outputChannel2)
		# self._logger.info("Output Channel 3: %s" % self.outputChannel3)
		# self._logger.info("Output Channel 4: %s" % self.outputChannel4)
		# self._logger.info("Output Channel 5: %s" % self.outputChannel5)
		# self._logger.info("Output Channel 6: %s" % self.outputChannel6)
		# self._logger.info("Input Channel 1: %s" % self.inputChannel1)
		# self._logger.info("Input Channel 2: %s" % self.inputChannel2)
		# self._logger.info("Input Channel 3: %s" % self.inputChannel3)
		# self._logger.info("Input Channel 4: %s" % self.inputChannel4)
		# self._logger.info("Input Channel 5: %s" % self.inputChannel5)
		# self._logger.info("Input Channel 6: %s" % self.inputChannel6)
		# self._logger.info("Input Channel 7: %s" % self.inputChannel7)
		# self._logger.info("Acceleration X: %s" % self.accelerationX)
		# self._logger.info("Acceleration Y: %s" % self.accelerationY)
		# self._logger.info("Acceleration Z: %s" % self.accelerationZ)
		# self._logger.info("Gyro X: %s" % self.gyroX)
		# self._logger.info("Gyro Y: %s" % self.gyroY)
		# self._logger.info("Gyro Z: %s" % self.gyroZ)
		self._logger.info("Kalman Reference Roll: %s" % self.kalmanReferenceValueRoll)
