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

try:
	from PyQt4 import QtGui
except:
	print "No PyQt found!"
	import sys
	sys.exit(2)

import pyqtgraph as pg
import numpy as np

class PlotCurve():
	def __init__(self, plot, field, fieldName):
		if not plot or not field or not fieldName:
			raise Exception, 'Argument exception!'

		self.plotDataItem = pg.PlotDataItem(name=fieldName)
		self.plot = plot
		# Data field
		self.dataField = field
		# Human readable data field name
		self.dataName = fieldName
		self.data = list()
		self.xData = list()	
		self.yData = list()
		self.curveColor = 'r'

		# This is the power to which each value must be raised
		self.scalePower = 0
		self.yMinimum = 0
		self.yMaximum = 0
		self.maxDataSize = 120 # Max number of data points

		self._enabled = True

	def drawCurve(self):
		"""Sets new data to the curve"""
		self.plotDataItem.setData(x=self.xData, y=self.yData)

	def updateData(self, boardStatus, timestamp):
		"""Updates the curve with new data. Is called when new data was received."""
		self.data.append((timestamp, boardStatus.getValue(self.dataField)))

		# Limit max size of data
		if len(self.data) > self.maxDataSize:
			self.data.pop(0)

		# Set new data
		self.xData = [float(s[0]) for s in self.data] # Time points
		self.yData = [float(s[1]) for s in self.data] # Value points

	def setCurveColor(self, color):
		if not color:
			return

		self.curveColor = color
		self.plotDataItem.setPen(color=color, width=1)

	def setMaxDataSize(self, size):
		"""Sets the maximum size (number of data points) of the X axis."""
		self.maxDataSize = size
