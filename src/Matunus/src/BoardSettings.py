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
	def __init__(self):
		self._logger = Logger()

		self.pidRollPFactor = 0
		self.pidRollIFactor = 0
		self.pidRollDFactor = 0
		self.pidPitchPFactor = 0
		self.pidPitchIFactor = 0
		self.pidPitchDFactor = 0
		self.pidYawPFactor = 0
		self.pidYawIFactor = 0
		self.pidYawDFactor = 0
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

	def updateFromMessage(self, message, timestamp):
		if message is None:
			raise Exception

		if message.commandType is not CommandTypes.GET_BOARD_SETTINGS:
			return

		self._lastUpdate = timestamp

		self.pidRollPFactor = message.pidRollPFactor
		self.pidRollIFactor = message.pidRollIFactor
		self.pidRollDFactor = message.pidRollDFactor
		self.pidPitchPFactor = message.pidPitchPFactor
		self.pidPitchIFactor = message.pidPitchIFactor
		self.pidPitchDFactor = message.pidPitchDFactor
		self.pidYawPFactor = message.pidYawPFactor
		self.pidYawIFactor = message.pidYawIFactor
		self.pidYawDFactor = message.pidYawDFactor
		self.kalmanRollQAngle = message.kalmanRollQAngle
		self.kalmanRollQGyro = message.kalmanRollQGyro
		self.kalmanRollRAngle = message.kalmanRollRAngle
		self.kalmanPitchQAngle = message.kalmanPitchQAngle
		self.kalmanPitchQGyro = message.kalmanPitchQGyro
		self.kalmanPitchRAngle = message.kalmanPitchRAngle
		self.kalmanYawQAngle = message.kalmanYawQAngle
		self.kalmanYawQGyro = message.kalmanYawQGyro
		self.kalmanYawRAngle = message.kalmanYawRAngle

	@property
	def outputChannelsLowerOffset(self):
		"""In microseconds"""
		return 490

	@property
	def inputChannelsOffset():
		"""In microseconds"""
		return 500

def BoardSettings():
	global boardSettings
	if not boardSettings:
		boardSettings = _BoardSettings()
	return boardSettings