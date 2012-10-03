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


from logger import Logger
from ApiCommands import CommandTypes


class BoardStatus(object):
	"""
	This class holds the current status of the flight controller.
	"""

	def __init__(self):
		self.dataFields = None
		self.lastUpdate = 0

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
		self.dataFields.append({'name': "accelerationX", 'value': 0})
		self.dataFields.append({'name': "accelerationY", 'value': 0})
		self.dataFields.append({'name': "accelerationZ", 'value': 0})
		self.dataFields.append({'name': "gyroX", 'value': 0})
		self.dataFields.append({'name': "gyroY", 'value': 0})
		self.dataFields.append({'name': "gyroZ", 'value': 0})

	def setField(self, name, value):
		"""Updates the specified field with the given value."""
		pass

	def getValue(self, name):
		pass

	def updateDataFromMessage(self, message, timestamp):
		"""
		This method takes a dictionary that contains the data values.
		"""
		if message is None:
			return

		if message.commandType == CommandTypes.GET_SERVO_POS:
			pass
		elif message.commandType == CommandTypes.GET_BOARD_STATUS:
			self.lastUpdate = timestamp
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
		else:
			return

		#self.hasNewData = True