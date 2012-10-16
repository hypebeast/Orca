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

try:
	import PyQt4.Qwt5 as Qwt 
except:
	print "No PyQwt found!"
	import sys
	sys.exit(2)


class PlotData:
	"""
	This class represents one plot curve and it's data.
	"""
	def __init__(self, plot, dataField):
		if not plot or not dataField:
			raise Exception

		self.plot = plot
		self.dataField = dataField
		self.data = list()
		self.xData = list()	
		self.yData = list()
		# This is the power to which each value must be raised
		self.scalePower = 0
		self.yMinimum = 0
		self.yMaximum = 0
		self.xWindowSize = 0
		self.curve = Qwt.QwtPlotCurve('')
		self.curve.setRenderHint(Qwt.QwtPlotItem.RenderAntialiased)
		pen = QtGui.QPen(QtGui.QColor('limegreen'))
		pen.setWidth(2)
		self.curve.setPen(pen)
		self.curve.attach(self.plot)
		
	def updateCurve(self):
		"""Sets new data to the curve"""
		# TODO: Adjust the axes scale
		self.curve.setData(self.xData, self.yData)

	def appendData(self, data):
		"""Appends new data to the plot"""
		self.data.append((data['timestamp'], data['value']))
		self.xData = [s[0] for s in self.data]
		self.yData = [s[1] for s in self.data]
