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


try:
    from PyQt4 import QtGui, QtCore, Qt
except ImportError:
    print "No PyQt found!"
    import sys
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
		- dataFields: 
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
		self.updateInterval = 0.5

		self.setCanvasBackground(Qt.Qt.black)
		self.setAxisTitle(Qwt.QwtPlot.xBottom, 'Time')
		self.setAxisScale(Qwt.QwtPlot.xBottom, 0, 10, 1)
		self.setAxisTitle(Qwt.QwtPlot.yLeft, 'Temperature')
		self.setAxisScale(Qwt.QwtPlot.yLeft, 0, 250, 40)
		self.replot()

		self.isRunning = False

		# Init all plot curves
		self._init_plots(dataFields)

	def start(self):
		"""
		Starts plotting.
		"""
		self.updateTimer.start(self.updateInterval * 1000.0)
		self.isRunning = True

	def stop(self):
		"""
		Stops plotting.
		"""
		self.isRunning = False

	def _init_plots(self, dataFields):
		"""
		Initializes all plots.
		"""
		for field in dataFields:
			self.plotCurves.append(PlotData(self, field))

	def _on_timer(self):
		"""
		Called by the timer.
		"""
		self._update_scope()

	def _update_scope(self):
		for curve in self.plotCurves:
			curve.updateCurve()

		self.replot()

	def _on_status_updated(self):
		if self.isRunning:
			data = dict(timestamp=self.boardStatus.lastUpdate,
						value=self.boardStatus.outputChannel1)
			self.plotCurves[0].appendData(data)