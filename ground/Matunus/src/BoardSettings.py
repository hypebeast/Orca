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


# Global FMU settings object
fmuSettings = None

class _FmuSettings():

	# Default Settings
	DEFAULT_PID_ROLL_PFACTOR = 0.05
	DEFAULT_PID_ROLL_IFACTOR = 0.002
	DEFAULT_PID_ROLL_DFACTOR = 0.00
	DEFAULT_PID_ROLL_ILIMIT = 5.0
	DEFAULT_DCM_ROLL_P_FACTOR = 0.2
	DEFAULT_DCM_ROLL_I_FACTOR = 0.0001
	DEFAULT_DCM_PITCH_P_FACTOR = 0.01

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
		self.dataFields.append(DataObjectField("dcmRollPFactor", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("dcmRollIFactor", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("dcmPitchPFactor", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("dcmPitchIFactor", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("dcmYawPFactor", "", DataFieldTypes.Float))
		self.dataFields.append(DataObjectField("dcmYawIFactor", "", DataFieldTypes.Float))

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
		self.setValue("dcmRollPFactor", self.DEFAULT_DCM_ROLL_P_FACTOR)
		self.setValue("dcmRollIFactor", self.DEFAULT_DCM_ROLL_I_FACTOR)
		self.setValue("dcmPitchPFactor", self.DEFAULT_DCM_PITCH_P_FACTOR)
		self.setValue("dcmPitchIFactor", 0)
		self.setValue("dcmYawPFactor", 0)
		self.setValue("dcmYawIFactor", 0)

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
		self.setValue("dcmRollPFactor", self.DEFAULT_DCM_ROLL_P_FACTOR)
		self.setValue("dcmRollIFactor", self.DEFAULT_DCM_ROLL_I_FACTOR)
		self.setValue("dcmPitchPFactor", self.DEFAULT_DCM_PITCH_P_FACTOR)
		self.setValue("dcmPitchIFactor", 0)
		self.setValue("dcmYawPFactor", 0)
		self.setValue("dcmYawIFactor", 0)

	def updateFromMessage(self, message, timestamp):
		if message is None:
			raise Exception

		if message.commandType is not CommandTypes.GET_BOARD_SETTINGS:
			return

		self._lastUpdate = timestamp

		for field in self.dataFields:
			if field.name == "pidRollPFactor":
				field.value = message.pidRollPFactor
				print message.pidRollPFactor
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
			elif field.name == "dcmRollPFactor":
				field.value = message.dcmRollPFactor
			elif field.name == "dcmRollIFactor":
				field.value = message.dcmRollIFactor
			elif field.name == "dcmPitchPFactor":
				field.value = message.dcmPitchPFactor
			elif field.name == "dcmPitchIFactor":
				field.value = message.dcmPitchIFactor
			elif field.name == "dcmYawPFactor":
				field.value = message.dcmYawPFactor
			elif field.name == "dcmYawIFactor":
				field.value = message.dcmYawIFactor

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

	def dcmRollPFactor():
	    doc = "The DCM roll P-Factor property."
	    def fget(self):
	        return self.getValue("dcmRollPFactor")
	    def fset(self, value):
	        self.setValue("dcmRollPFactor", value)
	    return locals()
	dcmRollPFactor = property(**dcmRollPFactor())

	def dcmRollIFactor():
	    doc = "The DCM roll I-Factor property."
	    def fget(self):
	        return self.getValue("dcmRollIFactor")
	    def fset(self, value):
	        self.setValue("dcmRollIFactor", value)
	    return locals()
	dcmRollIFactor = property(**dcmRollIFactor())

	def dcmPitchPFactor():
	    doc = "The DCM pitch P-Factor property."
	    def fget(self):
	        return self.getValue("dcmPitchPFactor")
	    def fset(self, value):
	        self.setValue("dcmPitchPFactor", value)
	    return locals()
	dcmPitchPFactor = property(**dcmPitchPFactor())

	def dcmPitchIFactor():
	    doc = "The DCM pitch I-Factor property."
	    def fget(self):
	        return self.getValue("dcmPitchIFactor")
	    def fset(self, value):
	        self.setValue("dcmPitchIFactor", value)
	    return locals()
	dcmPitchIFactor = property(**dcmPitchIFactor())

	def dcmYawPFactor():
	    doc = "The DCM yaaw P-Factor property."
	    def fget(self):
	        return self.getValue("dcmYawPFactor")
	    def fset(self, value):
	        self.setValue("dcmYawPFactor", value)
	    return locals()
	dcmYawPFactor = property(**dcmYawPFactor())

	def dcmYawIFactor():
	    doc = "The DCM yaw I-Factor property."
	    def fget(self):
	        return self.getValue("dcmYawIFactor")
	    def fset(self, value):
	        self.setValue("dcmYawIFactor", value)
	    return locals()
	dcmYawIFactor = property(**dcmYawIFactor())

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
		self._logger.info("Kalman Roll Q-Angle: %s" % self.dcmRollPFactor)
		self._logger.info("Kalman Roll Q-Gyro: %s" % self.dcmRollIFactor)
		self._logger.info("Kalman Roll R-Angle: %s" % self.dcmPitchPFactor)
		self._logger.info("--------------")


def FmuSettings():
	"""Singleton instance for the board settings"""
	global fmuSettings
	if not fmuSettings:
		fmuSettings = _FmuSettings()
	return fmuSettings