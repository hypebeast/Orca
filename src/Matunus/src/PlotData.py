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
	import PyQt4.Qwt5 as Qwt 
except:
	print "No PyQwt found!"
	import sys
	sys.exit(2)


class PlotData:
	"""
	Base class that keeps the data for each curve in the plot.
	"""
	def __init__(self, dataField):
		self.dataField = field
		self.data = list()
		# This is the power to which each value must be raised
		self.scalePower = 0
		self.yMinimum = 0
		self.yMaximum = 0
		self.xWindowSize
		self.curve = Qwt.QwtPlotCurve()
		self.xData = list()
		self.yData = list()

	def updatePlotCurveData():
		pass

	def append(self, data):
		"""Append new data to the plot"""
		pass