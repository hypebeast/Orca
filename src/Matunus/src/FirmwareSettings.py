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

# Global firmware settings object
firmwareSettings = None

class _FirmwareSettings(object):
	"""
	This class handles reading and writing of the firmware settings.
	"""
	def __init__(self):
		self._logger = Logger()

	@property
	def outputChannelsLowerOffset(self):
		"""In microseconds"""
		return 490

	@property
	def inputChannelsOffset():
		"""In microseconds"""
		return 500


def FirmwareSettings():
	global firmwareSettings
	if not firmwareSettings:
		firmwareSettings = _FirmwareSettings()
	return firmwareSettings