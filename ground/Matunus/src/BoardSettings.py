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
from DataObjectField import DataObjectField, DataFieldTypes


# Global firmware settings object
boardSettings = None

class _BoardSettings():

	# Default Settings
	DEFAULT_PID_ROLL_PFACTOR = 0.05
	DEFAULT_PID_ROLL_IFACTOR = 0.002
	DEFAULT_PID_ROLL_DFACTOR = 0.00
	DEFAULT_PID_ROLL_ILIMIT = 5.0
	DEFAULT_KALMAN_ROLL_QAngle = 0.2
	DEFAULT_KALMAN_ROLL_QGyro = 0.0001
	DEFAULT_KALMAN_ROLL_RAngle = 0.01

	def __init__(self):
		self._logger = Logger()
		self.dataFields = list()
		self._lastUpdate = 0

		# Create data fields
		self._createDataFields()

		# Set default values
		self._setDefaults()

	def _createDataFields(self):
		"""
		Creates all data fields.
		"""
		self.dataFields.append(DataObjectField("pidRollPFactor", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("pidRollIFactor", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("pidRollDFactor", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("pidRollILimit", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("pidPitchPFactor", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("pidPitchIFactor", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("pidPitchDFactor", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("pidPitchILimit", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("pidYawPFactor", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("pidYawIFactor", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("pidYawDFactor", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("pidYawILimit", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("kalmanRollQAngle", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("kalmanRollQGyro", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("kalmanRollRAngle", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("kalmanPitchQAngle", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("kalmanPitchQGyro", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("kalmanPitchRAngle", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("kalmanYawQAngle", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("kalmanYawQGyro", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("kalmanYawRAngle", "", DataFieldTypes.Float))

	def setValue(self, name, value):
		"""Updates the specified field with the given value."""
		for field in self.dataFields:
			if field.name == name:
				field.value = value
				break

	def getValue(self, name):
		"""Returns the value of the specified data field."""
		for field in self.dataFields:
			if field.name == name:
				return field.value

		return None

	def _setDefaults(self):
		self.setValue("pidRollPFactor", self.DEFAULT_PID_ROLL_PFACTOR)
		self.setValue("pidRollIFactor", self.DEFAULT_PID_ROLL_IFACTOR)
		self.setValue("pidRollDFactor", self.DEFAULT_PID_ROLL_DFACTOR)
		self.setValue("pidRollILimit", self.DEFAULT_PID_ROLL_ILIMIT)
		self.setValue("pidPitchPFactor", 0)
		self.setValue("pidPitchIFactor", 0)
		self.setValue("pidPitchDFactor", 0)
		self.setValue("pidPitchILimit", 0)
		self.setValue("pidYawPFactor", 0)
		self.setValue("pidYawIFactor", 0)
		self.setValue("pidYawDFactor", 0)
		self.setValue("pidYawILimit", 0)
		self.setValue("kalmanRollQAngle", self.DEFAULT_KALMAN_ROLL_QAngle)
		self.setValue("kalmanRollQGyro", self.DEFAULT_KALMAN_ROLL_QGyro)
		self.setValue("kalmanRollRAngle", self.DEFAULT_KALMAN_ROLL_RAngle)
		self.setValue("kalmanPitchQAngle", 0)
		self.setValue("kalmanPitchQGyro", 0)
		self.setValue("kalmanPitchRAngle", 0)
		self.setValue("kalmanYawQAngle", 0)
		self.setValue("kalmanYawQGyro", 0)
		self.setValue("kalmanYawRAngle", 0)

	def restoreFactorySettings(self):
		self._setDefaults()

	def restoreStabilizationFactorySettings(self):
		self.setValue("pidRollPFactor", self.DEFAULT_PID_ROLL_PFACTOR)
		self.setValue("pidRollIFactor", self.DEFAULT_PID_ROLL_IFACTOR)
		self.setValue("pidRollDFactor", self.DEFAULT_PID_ROLL_DFACTOR)
		self.setValue("pidRollILimit", self.DEFAULT_PID_ROLL_ILIMIT)
		self.setValue("pidPitchPFactor", 0)
		self.setValue("pidPitchIFactor", 0)
		self.setValue("pidPitchDFactor", 0)
		self.setValue("pidPitchILimit", 0)
		self.setValue("pidYawPFactor", 0)
		self.setValue("pidYawIFactor", 0)
		self.setValue("pidYawDFactor", 0)
		self.setValue("pidYawILimit", 0)
		self.setValue("kalmanRollQAngle", self.DEFAULT_KALMAN_ROLL_QAngle)
		self.setValue("kalmanRollQGyro", self.DEFAULT_KALMAN_ROLL_QGyro)
		self.setValue("kalmanRollRAngle", self.DEFAULT_KALMAN_ROLL_RAngle)
		self.setValue("kalmanPitchQAngle", 0)
		self.setValue("kalmanPitchQGyro", 0)
		self.setValue("kalmanPitchRAngle", 0)
		self.setValue("kalmanYawQAngle", 0)
		self.setValue("kalmanYawQGyro", 0)
		self.setValue("kalmanYawRAngle", 0)

	def updateFromMessage(self, message, timestamp):
		if message is None:
			raise Exception

		if message.commandType is not CommandTypes.GET_BOARD_SETTINGS:
			return

		self._lastUpdate = timestamp

		for field in self.dataFields:
			if field.name == "pidRollPFactor":
				field.value = message.pidRollPFactor
			elif field.name == "pidRollIFactor":
				field.value = message.pidRollIFactor
			elif field.name == "pidRollDFactor":
				field.value = message.pidRollDFactor
			elif field.name == "pidRollILimit":
				field.value = message.pidRollILimit
			elif field.name == "pidPitchPFactor":
				field.value = message.pidPitchPFactor
			elif field.name == "pidPitchIFactor":
				field.value = message.pidPitchIFactor
			elif field.name == "pidPitchDFactor":
				field.value = message.pidPitchDFactor
			elif field.name == "pidPitchILimit":
				field.value = message.pidPitchILimit
			elif field.name == "pidYawPFactor":
				field.value = message.pidYawPFactor
			elif field.name == "pidYawIFactor":
				field.value = message.pidYawIFactor
			elif field.name == "pidYawDFactor":
				field.value = message.pidYawDFactor
			elif field.name == "pidYawILimit":
				field.value = message.pidYawILimit
			elif field.name == "kalmanRollQAngle":
				field.value = message.kalmanRollQAngle
			elif field.name == "kalmanRollQGyro":
				field.value = message.kalmanRollQGyro
			elif field.name == "kalmanRollRAngle":
				field.value = message.kalmanRollRAngle
			elif field.name == "kalmanPitchQAngle":
				field.value = message.kalmanPitchQAngle
			elif field.name == "kalmanPitchQGyro":
				field.value = message.kalmanPitchQGyro
			elif field.name == "kalmanPitchRAngle":
				field.value = message.kalmanPitchRAngle
			elif field.name == "kalmanYawQAngle":
				field.value = message.kalmanYawQAngle
			elif field.name == "kalmanYawQGyro":
				field.value = message.kalmanYawQGyro
			elif field.name == "kalmanYawRAngle":
				field.value = message.kalmanYawRAngle

		#self.printStatus()
	
	####
	## Properties
	####
	def pidRollPFactor():
	    doc = "The PID Roll P-Factor property."
	    def fget(self):
	        return self.getValue("pidRollPFactor")
	    def fset(self, value):
	        self.setValue("pidRollPFactor", value)
	    return locals()
	pidRollPFactor = property(**pidRollPFactor())

	def pidRollIFactor():
	    doc = "The PID Roll I-Factor property."
	    def fget(self):
	        return self.getValue("pidRollIFactor")
	    def fset(self, value):
	        self.setValue("pidRollIFactor", value)
	    return locals()
	pidRollIFactor = property(**pidRollIFactor())

	def pidRollDFactor():
	    doc = "The PID Roll D-Factor property."
	    def fget(self):
	        return self.getValue("pidRollDFactor")
	    def fset(self, value):
	        self.setValue("pidRollDFactor", value)
	    return locals()
	pidRollDFactor = property(**pidRollDFactor())

	def pidRollILimit():
	    doc = "The PID Roll I-Limit property."
	    def fget(self):
	        return self.getValue("pidRollILimit")
	    def fset(self, value):
	        self.setValue("pidRollILimit", value)
	    return locals()
	pidRollILimit = property(**pidRollILimit())

	def pidPitchPFactor():
	    doc = "The PID Pitch P-Factor property."
	    def fget(self):
	        return self.getValue("pidPitchPFactor")
	    def fset(self, value):
	        self.setValue("pidPitchPFactor", value)
	    return locals()
	pidPitchPFactor = property(**pidPitchPFactor())

	def pidPitchIFactor():
	    doc = "The PID Pitch I-Factor property."
	    def fget(self):
	        return self.getValue("pidPitchIFactor")
	    def fset(self, value):
	        self.setValue("pidPitchIFactor", value)
	    return locals()
	pidPitchIFactor = property(**pidPitchIFactor())

	def pidPitchDFactor():
	    doc = "The PID Pitch D-Factor property."
	    def fget(self):
	        return self.getValue("pidPitchDFactor")
	    def fset(self, value):
	        self.setValue("pidPitchDFactor", value)
	    return locals()
	pidPitchDFactor = property(**pidPitchDFactor())

	def pidPitchILimit():
	    doc = "The PID Pitch I-Limit property."
	    def fget(self):
	        return self.getValue("pidPitchILimit")
	    def fset(self, value):
	        self.setValue("pidPitchILimit", value)
	    return locals()
	pidPitchILimit = property(**pidPitchILimit())

	def pidYawPFactor():
	    doc = "The PID Yaw P-Factor property."
	    def fget(self):
	        return self.getValue("pidYawPFactor")
	    def fset(self, value):
	        self.setValue("pidYawPFactor", value)
	    return locals()
	pidYawPFactor = property(**pidYawPFactor())

	def pidYawIFactor():
	    doc = "The PID Yaw I-Factor property."
	    def fget(self):
	        return self.getValue("pidYawIFactor")
	    def fset(self, value):
	        self.setValue("pidYawIFactor", value)
	    return locals()
	pidYawIFactor = property(**pidYawIFactor())

	def pidYawDFactor():
	    doc = "The PID Yaw D-Factor property."
	    def fget(self):
	        return self.getValue("pidYawDFactor")
	    def fset(self, value):
	        self.setValue("pidYawDFactor", value)
	    return locals()
	pidYawDFactor = property(**pidYawDFactor())

	def pidYawILimit():
	    doc = "The PID Yaw I-Limit property."
	    def fget(self):
	        return self.getValue("pidYawILimit")
	    def fset(self, value):
	        self.setValue("pidYawILimit", value)
	    return locals()
	pidYawILimit = property(**pidYawILimit())

	def kalmanRollQAngle():
	    doc = "The Kalman roll Q-Angle property."
	    def fget(self):
	        return self.getValue("kalmanRollQAngle")
	    def fset(self, value):
	        self.setValue("kalmanRollQAngle", value)
	    return locals()
	kalmanRollQAngle = property(**kalmanRollQAngle())

	def kalmanRollQGyro():
	    doc = "The Kalman roll Q-Angle property."
	    def fget(self):
	        return self.getValue("kalmanRollQGyro")
	    def fset(self, value):
	        self.setValue("kalmanRollQGyro", value)
	    return locals()
	kalmanRollQGyro = property(**kalmanRollQGyro())

	def kalmanRollRAngle():
	    doc = "The Kalman roll Q-Angle property."
	    def fget(self):
	        return self.getValue("kalmanRollRAngle")
	    def fset(self, value):
	        self.setValue("kalmanRollRAngle", value)
	    return locals()
	kalmanRollRAngle = property(**kalmanRollRAngle())

	def kalmanPitchQAngle():
	    doc = "The Kalman pitch Q-Angle property."
	    def fget(self):
	        return self.getValue("kalmanPitchQAngle")
	    def fset(self, value):
	        self.setValue("kalmanPitchQAngle", value)
	    return locals()
	kalmanPitchQAngle = property(**kalmanPitchQAngle())

	def kalmanPitchQGyro():
	    doc = "The Kalman pitch Q-Angle property."
	    def fget(self):
	        return self.getValue("kalmanPitchQGyro")
	    def fset(self, value):
	        self.setValue("kalmanPitchQGyro", value)
	    return locals()
	kalmanPitchQGyro = property(**kalmanPitchQGyro())

	def kalmanPitchRAngle():
	    doc = "The Kalman pitch Q-Angle property."
	    def fget(self):
	        return self.getValue("kalmanPitchRAngle")
	    def fset(self, value):
	        self.setValue("kalmanPitchRAngle", value)
	    return locals()
	kalmanPitchRAngle = property(**kalmanPitchRAngle())

	def kalmanYawQAngle():
	    doc = "The Kalman yaw Q-Angle property."
	    def fget(self):
	        return self.getValue("kalmanYawQAngle")
	    def fset(self, value):
	        self.setValue("kalmanYawQAngle", value)
	    return locals()
	kalmanYawQAngle = property(**kalmanYawQAngle())

	def kalmanYawQGyro():
	    doc = "The Kalman yaw Q-Angle property."
	    def fget(self):
	        return self.getValue("kalmanYawQGyro")
	    def fset(self, value):
	        self.setValue("kalmanYawQGyro", value)
	    return locals()
	kalmanYawQGyro = property(**kalmanYawQGyro())

	def kalmanYawRAngle():
	    doc = "The Kalman yaw Q-Angle property."
	    def fget(self):
	        return self.getValue("kalmanYawRAngle")
	    def fset(self, value):
	        self.setValue("kalmanYawRAngle", value)
	    return locals()
	kalmanYawRAngle = property(**kalmanYawRAngle())

	def printSettings(self):
		"""
		Prints all settings values.
		"""
		self._logger.info("Board Settings")
		self._logger.info("--------------")
		self._logger.info("PID Roll P-Factor: %s" % self.pidRollPFactor)
		self._logger.info("PID Roll I-Factor: %s" % self.pidRollIFactor)
		self._logger.info("PID Roll D-Factor: %s" % self.pidRollDFactor)
		self._logger.info("PID Roll I-Limit: %s" % self.pidRollILimit)
		self._logger.info("Kalman Roll Q-Angle: %s" % self.kalmanRollQAngle)
		self._logger.info("Kalman Roll Q-Gyro: %s" % self.kalmanRollQGyro)
		self._logger.info("Kalman Roll R-Angle: %s" % self.kalmanRollRAngle)
		self._logger.info("--------------")


def BoardSettings():
	"""Singleton instance for the board settings"""
	global boardSettings
	if not boardSettings:
		boardSettings = _BoardSettings()
	return boardSettings