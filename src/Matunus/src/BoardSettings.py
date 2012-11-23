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

		self.pidRollPFactor = 0
		self.pidRollIFactor = 0
		self.pidRollDFactor = 0
		self.pidRollILimit = 0
		self.pidPitchPFactor = 0
		self.pidPitchIFactor = 0
		self.pidPitchDFactor = 0
		self.pidPitchILimit = 0
		self.pidYawPFactor = 0
		self.pidYawIFactor = 0
		self.pidYawDFactor = 0
		self.pidYawILimit = 0
		self.kalmanRollQAngle = 0
		self.kalmanRollQGyro = 0
		self.kalmanRollRAngle = 0
		self.kalmanPitchQAngle = 0
		self.kalmanPitchQGyro = 0
		self.kalmanPitchRAngle = 0
		self.kalmanYawQAngle = 0
		self.kalmanYawQGyro = 0
		self.kalmanYawRAngle = 0

		self._lastUpdate = 0

		self._setDefaults()

	def _setDefaults(self):
		self.pidRollPFactor = self.DEFAULT_PID_ROLL_PFACTOR
		self.pidRollIFactor = self.DEFAULT_PID_ROLL_IFACTOR
		self.pidRollDFactor = self.DEFAULT_PID_ROLL_DFACTOR
		self.pidRollILimit = self.DEFAULT_PID_ROLL_ILIMIT
		self.pidPitchPFactor = 0
		self.pidPitchIFactor = 0
		self.pidPitchDFactor = 0
		self.pidPitchILimit = 0
		self.pidYawPFactor = 0
		self.pidYawIFactor = 0
		self.pidYawDFactor = 0
		self.pidYawILimit = 0
		self.kalmanRollQAngle = self.DEFAULT_KALMAN_ROLL_QAngle
		self.kalmanRollQGyro = self.DEFAULT_KALMAN_ROLL_QGyro
		self.kalmanRollRAngle = self.DEFAULT_KALMAN_ROLL_RAngle
		self.kalmanPitchQAngle = 0
		self.kalmanPitchQGyro = 0
		self.kalmanPitchRAngle = 0
		self.kalmanYawQAngle = 0
		self.kalmanYawQGyro = 0
		self.kalmanYawRAngle = 0

	def restoreFactorySettings(self):
		self._setDefaults()

	def restoreStabilizationFactorySettings(self):
		self.pidRollPFactor = self.DEFAULT_PID_ROLL_PFACTOR
		self.pidRollIFactor = self.DEFAULT_PID_ROLL_IFACTOR
		self.pidRollDFactor = self.DEFAULT_PID_ROLL_DFACTOR
		self.pidRollILimit = self.DEFAULT_PID_ROLL_ILIMIT
		self.pidPitchPFactor = 0
		self.pidPitchIFactor = 0
		self.pidPitchDFactor = 0
		self.pidPitchILimit = 0
		self.pidYawPFactor = 0
		self.pidYawIFactor = 0
		self.pidYawDFactor = 0
		self.pidYawILimit = 0
		self.kalmanRollQAngle = self.DEFAULT_KALMAN_ROLL_QAngle
		self.kalmanRollQGyro = self.DEFAULT_KALMAN_ROLL_QGyro
		self.kalmanRollRAngle = self.DEFAULT_KALMAN_ROLL_RAngle
		self.kalmanPitchQAngle = 0
		self.kalmanPitchQGyro = 0
		self.kalmanPitchRAngle = 0
		self.kalmanYawQAngle = 0
		self.kalmanYawQGyro = 0
		self.kalmanYawRAngle = 0

	def updateFromMessage(self, message, timestamp):
		if message is None:
			raise Exception

		if message.commandType is not CommandTypes.GET_BOARD_SETTINGS:
			return

		self._lastUpdate = timestamp

		self.pidRollPFactor = message.pidRollPFactor
		self.pidRollIFactor = message.pidRollIFactor
		self.pidRollDFactor = message.pidRollDFactor
		self.pidRollILimit = message.pidRollILimit
		self.pidPitchPFactor = message.pidPitchPFactor
		self.pidPitchIFactor = message.pidPitchIFactor
		self.pidPitchDFactor = message.pidPitchDFactor
		self.pidPitchILimit = message.pidPitchILimit
		self.pidYawPFactor = message.pidYawPFactor
		self.pidYawIFactor = message.pidYawIFactor
		self.pidYawDFactor = message.pidYawDFactor
		self.pidYawILimit = message.pidYawILimit
		self.kalmanRollQAngle = message.kalmanRollQAngle
		self.kalmanRollQGyro = message.kalmanRollQGyro
		self.kalmanRollRAngle = message.kalmanRollRAngle
		self.kalmanPitchQAngle = message.kalmanPitchQAngle
		self.kalmanPitchQGyro = message.kalmanPitchQGyro
		self.kalmanPitchRAngle = message.kalmanPitchRAngle
		self.kalmanYawQAngle = message.kalmanYawQAngle
		self.kalmanYawQGyro = message.kalmanYawQGyro
		self.kalmanYawRAngle = message.kalmanYawRAngle

		#self.printStatus()

	@property
	def outputChannelsLowerOffset(self):
		"""In microseconds"""
		return 490

	@property
	def inputChannelsOffset():
		"""In microseconds"""
		return 500

	def printStatus(self):
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