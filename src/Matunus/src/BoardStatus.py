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


from ApiCommands import CommandTypes
from logger import Logger


class BoardStatus(object):
	"""
	This class holds the current status of the flight controller.
	"""
	def __init__(self):
		self.dataFields = None
		self._lastUpdate = 0

		# Logger
		self._logger = Logger()

		# Add all fields
		self._addFields()

	def _addFields(self):
		"""Creates and adds all data fields."""
		self.dataFields = list()

		self.dataFields.append({'name': "outputChannel1", 'value': 0})
		self.dataFields.append({'name': "outputChannel2", 'value': 0})
		self.dataFields.append({'name': "outputChannel3", 'value': 0})
		self.dataFields.append({'name': "outputChannel4", 'value': 0})
		self.dataFields.append({'name': "outputChannel5", 'value': 0})
		self.dataFields.append({'name': "outputChannel6", 'value': 0})
		self.dataFields.append({'name': "inputChannel1", 'value': 0})
		self.dataFields.append({'name': "inputChannel2", 'value': 0})
		self.dataFields.append({'name': "inputChannel3", 'value': 0})
		self.dataFields.append({'name': "inputChannel4", 'value': 0})
		self.dataFields.append({'name': "inputChannel5", 'value': 0})
		self.dataFields.append({'name': "inputChannel6", 'value': 0})
		self.dataFields.append({'name': "inputChannel7", 'value': 0})
		self.dataFields.append({'name': "accelerationX", 'value': 0})
		self.dataFields.append({'name': "accelerationY", 'value': 0})
		self.dataFields.append({'name': "accelerationZ", 'value': 0})
		self.dataFields.append({'name': "gyroX", 'value': 0})
		self.dataFields.append({'name': "gyroY", 'value': 0})
		self.dataFields.append({'name': "gyroZ", 'value': 0})
		self.dataFields.append({'name': "kalmanOutputRoll", 'value': 0})
		self.dataFields.append({'name': "kalmanReferenceValueRoll", 'value': 0})
		self.dataFields.append({'name': "setValueRollAngle", 'value': 0})
		self.dataFields.append({'name': "actuatingVariablePidRoll", 'value': 0})

	def setField(self, name, value):
		"""Updates the specified field with the given value."""
		pass

	def getValue(self, name):
		"""Returns the value of the specified data field."""
		for field in self.dataFields:
			if field['name'] == name:
				return field['value']

		return None

	def updateFromMessage(self, message, timestamp):
		"""
		This method takes a dictionary that contains the data values.
		"""
		if message is None:
			return

		if message.commandType == CommandTypes.GET_SERVO_POS:
			pass
		elif message.commandType == CommandTypes.GET_BOARD_STATUS:
			self._lastUpdate = timestamp
			for field in self.dataFields:
				if field['name'] == "outputChannel1":
					field['value'] = message.outputChannel1
				elif field['name'] == "outputChannel2":
					field['value'] = message.outputChannel2
				elif field['name'] == "outputChannel3":
					field['value'] = message.outputChannel3
				elif field['name'] == "outputChannel4":
					field['value'] = message.outputChannel4
				elif field['name'] == "outputChannel5":
					field['value'] = message.outputChannel5
				elif field['name'] == "outputChannel6":
					field['value'] = message.outputChannel6
				elif field['name'] == "inputChannel1":
					field['value'] = message.inputChannel1
				elif field['name'] == "inputChannel2":
					field['value'] = message.inputChannel2
				elif field['name'] == "inputChannel3":
					field['value'] = message.inputChannel3
				elif field['name'] == "inputChannel4":
					field['value'] = message.inputChannel4
				elif field['name'] == "inputChannel5":
					field['value'] = message.inputChannel5
				elif field['name'] == "inputChannel6":
					field['value'] = message.inputChannel6
				elif field['name'] == "inputChannel7":
					field['value'] = message.inputChannel7
				elif field['name'] == "accelerationX":
					field['value'] = message.accelerationX
				elif field['name'] == "accelerationY":
					field['value'] = message.accelerationY
				elif field['name'] == "accelerationZ":
					field['value'] = message.accelerationZ
				elif field['name'] == "gyroX":
					field['value'] = message.gyroX
				elif field['name'] == "gyroY":
					field['value'] = message.gyroY
				elif field['name'] == "gyroZ":
					field['value'] = message.gyroZ
				elif field['name'] == "kalmanOutputRoll":
					field['value'] = message.kalmanOutputRoll
				elif field['name'] == "kalmanReferenceValueRoll":
					field['value'] = message.kalmanReferenceValueRoll
				elif field['name'] == "setValueRollAngle":
					field['value'] = message.setValueRollAngle
				elif field['name'] == "actuatingVariablePidRoll":
					field['value'] = message.actuatingVariablePidRoll
		else:
			return

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
	def lastUpdate(self):
		return self._lastUpdate

	def printStatus(self):
		"""
		Prints the current status to stdout.
		"""
		self._logger.info("Board Status")
		self._logger.info("--------------------------")
		self._logger.info("\tOutput Channel 1: %s" % self.outputChannel1)
		self._logger.info("\tOutput Channel 2: %s" % self.outputChannel2)
		self._logger.info("\tOutput Channel 3: %s" % self.outputChannel3)
		self._logger.info("\tOutput Channel 4: %s" % self.outputChannel4)
		self._logger.info("\tOutput Channel 5: %s" % self.outputChannel5)
		self._logger.info("\tOutput Channel 6: %s" % self.outputChannel6)
		self._logger.info("\tInput Channel 1: %s" % self.inputChannel1)
		self._logger.info("\tInput Channel 2: %s" % self.inputChannel2)
		self._logger.info("\tInput Channel 3: %s" % self.inputChannel3)
		self._logger.info("\tInput Channel 4: %s" % self.inputChannel4)
		self._logger.info("\tInput Channel 5: %s" % self.inputChannel5)
		self._logger.info("\tInput Channel 6: %s" % self.inputChannel6)
		self._logger.info("\tInput Channel 7: %s" % self.inputChannel7)
		self._logger.info("\tAcceleration X: %s" % self.accelerationX)
		self._logger.info("\tAcceleration Y: %s" % self.accelerationY)
		self._logger.info("\tAcceleration Z: %s" % self.accelerationZ)
		self._logger.info("\tGyro X: %s" % self.gyroX)
		self._logger.info("\tGyro Y: %s" % self.gyroY)
		self._logger.info("\tGyro Z: %s" % self.gyroZ)
