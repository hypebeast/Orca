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


import time
import sys

try:
    from PyQt4 import QtCore, Qt
except ImportError:
    print "No PyQt found!"
    sys.exit(2)

try:
	import PyQt4.Qwt5 as Qwt 
except:
	print "No PyQwt found!"
	import sys
	sys.exit(2)

from PlotData import PlotData


class ScopeWidget(Qwt.QwtPlot):
	"""
	Widget that contains one scope.
	"""
	def __init__(self, boardController, dataFields):
		"""
		Constructor

		Params:

		- boardController: The board controller object
		- dataFields: Tumple with the names of the data fields which should be scoped
		"""
		Qwt.QwtPlot.__init__(self)

		if dataFields is None or boardController is None:
			raise "Error!"

		self.boardController = boardController
		self.boardController.board_status_updated.connect(self._on_status_updated)
		self.boardStatus = self.boardController.boardStatus
		self.plotCurves = list()
		
		# This timer updates the plot curves
		self.updateTimer = QtCore.QTimer()
		QtCore.QObject.connect(self.updateTimer, QtCore.SIGNAL('timeout()'), self._on_timer)
		self._updateInterval = 0.5
		self._scopeLength = 60 # Scope length in seconds

		# Basic plot setup
		self.setCanvasBackground(Qt.Qt.black)
		self.setAxisTitle(Qwt.QwtPlot.xBottom, 'Time (seconds)')
		self.setAxisScale(Qwt.QwtPlot.xBottom, 0, self._scopeLength+5, 1)
		self.setAxisTitle(Qwt.QwtPlot.yLeft, 'Values')
		self.setAxisScale(Qwt.QwtPlot.yLeft, 0, 250, 50)
		
		# Insert legend		
		self._legend = Qwt.QwtLegend()
		self.insertLegend(self._legend, Qwt.QwtPlot.BottomLegend)

		# Grid
		grid = Qwt.QwtPlotGrid()
		grid.setPen(Qt.QPen(Qt.Qt.gray, 1, Qt.Qt.DotLine))
		grid.attach(self)

		self._isRunning = False
		self._startTime = 0

		# Assign data fields
		self.dataFields = dataFields

		# Init zoom
		self._initZoomer()

		# Init all plot curves
		self._init_plots()

	def _initZoomer(self):
		"""Initialize zooming
        """
		self.zoomer = Qwt.QwtPlotZoomer(Qwt.QwtPlot.xBottom,
                                        Qwt.QwtPlot.yLeft,
                                        Qwt.QwtPicker.DragSelection,
                                        Qwt.QwtPicker.AlwaysOff,
                                        self.canvas())
		self.zoomer.setRubberBandPen(Qt.QPen(Qt.Qt.white))

	def setZoomerMousePattern(self, index):
		"""Set the mouse zoomer pattern.
		"""

		if index == 0:
			pattern = [
                Qwt.QwtEventPattern.MousePattern(Qt.Qt.LeftButton,
                                                 Qt.Qt.NoModifier),
                Qwt.QwtEventPattern.MousePattern(Qt.Qt.MidButton,
                                                 Qt.Qt.NoModifier),
                Qwt.QwtEventPattern.MousePattern(Qt.Qt.RightButton,
                                                 Qt.Qt.NoModifier),
                Qwt.QwtEventPattern.MousePattern(Qt.Qt.LeftButton,
                                                 Qt.Qt.ShiftModifier),
                Qwt.QwtEventPattern.MousePattern(Qt.Qt.MidButton,
                                                 Qt.Qt.ShiftModifier),
                Qwt.QwtEventPattern.MousePattern(Qt.Qt.RightButton,
                                                 Qt.Qt.ShiftModifier),
                ]
			self.zoomer.setMousePattern(pattern)
		elif index in (1, 2, 3):
			self.zoomer.initMousePattern(index)
		else:
			raise ValueError, 'index must be in (0, 1, 2, 3)'

	def start(self):
		"""
		Starts plotting.
		"""
		self._init_plots()
		self._startTime = time.clock()
		self.updateTimer.start(self._updateInterval * 1000.0)
		self._isRunning = True

	def stop(self):
		"""
		Stops plotting.
		"""
		self._startTime = 0
		self.updateTimer.stop()
		self._isRunning = False

	def setDataFields(self, dataFields):
		if self._isRunning:
			return

		self.dataFields = dataFields
		self._init_plots()

	def _init_plots(self):
		"""
		Initializes all plots.
		"""
		colors = [Qt.Qt.red, Qt.Qt.green, Qt.Qt.blue, Qt.Qt.cyan, Qt.Qt.magenta,
				  Qt.Qt.yellow, '#997B59', Qt.Qt.gray]
		self.plotCurves = list()
		# Remove all attached curves from the plot
		self.detachItems()
		fields = self.dataFields['fields']
		names = self.dataFields['fieldNames']
		colorIndex = 0
		# Add for every data field one plot curve
		for i in range(0, len(fields)):
			curve = PlotData(self, fields[i], names[i])
			# Set color
			curve.setCurveColor(colors[colorIndex])
			# Set the max number of data points
			curve.setXAxisSize(self._scopeLength/self._updateInterval)
			self.plotCurves.append(curve)
			colorIndex += 1
			if colorIndex >= len(colors):
				colorIndex = 0

		# Legend
		for curve in self.plotCurves:
			self._legend.find(curve.curve).setItemMode(Qwt.QwtLegend.CheckableItem)
			self._legend.find(curve.curve).setChecked(True)

		self.legendChecked.connect(self._onLegendChecked)

	def _on_timer(self):
		"""
		Called by the timer.
		"""
		# Update the scope
		self._update_scope()

	def _update_scope(self):
		"""Updates every scope curve and replots everything."""
		# Set scale for the x-Axis and y-Axis
		if len(self.plotCurves) > 0:
			# Find max y-value of all curves
			max_yVal = -sys.maxint - 1
			for curve in self.plotCurves:
				val = max(curve.yData)
				if val > max_yVal:
					max_yVal = val

			# Find min y-value of all curves
			min_yVal = sys.maxint
			for curve in self.plotCurves:
				val = min(curve.yData)
				if val < min_yVal:
					min_yVal = val

			# X-Axis
			self.setAxisScale(Qwt.QwtPlot.xBottom, self.plotCurves[0].xData[0],
				max(self._scopeLength, self.plotCurves[0].xData[-1]), 1)
			# Y-Axis
			self.setAxisScale(Qwt.QwtPlot.yLeft, min_yVal,
				max_yVal, int((max_yVal - min_yVal) / 20))

		# Draw every curve
		for curve in self.plotCurves:
			curve.drawCurve()

		# Replot everything
		self.replot()

	def _on_status_updated(self):
		"""Updates the plot data. Called when the board status was updated."""
		if self._isRunning:
			timestamp = time.clock() - self._startTime
			for curve in self.plotCurves:
				curve.updateData(self.boardStatus, timestamp)

	def _onLegendChecked(self, plotItem, on):
		if on: plotItem.show()
		else: plotItem.hide()
