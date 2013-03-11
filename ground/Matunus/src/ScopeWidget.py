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

import time, sys

try:
    from PyQt4 import QtCore, QtGui
except ImportError:
    print "No PyQt found!"
    sys.exit(2)

import pyqtgraph as pg

from PlotCurve import PlotCurve


class ScopeWidget(QtGui.QWidget):
    """Scope widget based on pyqtgraph"""
    def __init__(self, fmuManager, dataFields):
        super(ScopeWidget, self).__init__()

        if not dataFields and not fmuManager:
            raise Exception, 'No FMU manager or data field specified!'

        self._fmuManager = fmuManager
        self._fmuManager.vtol_object_received.connect(self._on_vtol_object_received)
        self.plotCurves = list()
        self.plotWidget = pg.PlotWidget()
        self.plotItem = self.plotWidget.getPlotItem()
        self.legend = None
        self.legendEnabled = False
        self.curveButtons = list()
        self.curveColorsButton = ['#FF0000', '#00FF00', '#0000FF',
                                    '#00FFFF', '#FF00FF', '#FFFF00',
                                    '#997B59', '#808080']
        self.curveColors = ['r', 'g', 'b', 'c', 'm', 'y', '997B59', '808080']

        # Signal mapper for the curve buttons
        self.signalMapper = QtCore.QSignalMapper(self)
        QtCore.QObject.connect(self.signalMapper, QtCore.SIGNAL("mapped(int)"), self._onCurveButtonToggled)

        # This timer updates the plot curves
        self.updateTimer = QtCore.QTimer()
        QtCore.QObject.connect(self.updateTimer, QtCore.SIGNAL('timeout()'), self._on_timer)
        self._updateInterval = 0.25
        self._scopeLength = 60  # Scope length in seconds
        self._isRunning = False
        self._startTime = 0

        # Data fields
        self.dataFields = dataFields

        # Initialize the UI
        self._initUi()

        # Initialize the plot and all curves
        self._initPlot()

    def _initUi(self):
        self.mainLayout = QtGui.QVBoxLayout()
        self.setLayout(self.mainLayout)
        self.mainLayout.addWidget(self.plotWidget)

        self.curveButtonsLayout = QtGui.QHBoxLayout()
        self.curveButtonsLayout.setSpacing(10)
        self.mainLayout.addLayout(self.curveButtonsLayout)

    def _initPlot(self):
        # Set axis titles
        self.plotWidget.setLabel('left', 'Value', units='V')
        self.plotWidget.setLabel('bottom', 'Time', units='s')

        # Set initial scale/range for x- and y-axis
        self.plotWidget.setRange(xRange=(0, self._scopeLength + 5), yRange=(0, 100))

        # Disable autorange for the y-axis
        self.plotWidget.disableAutoRange(axis=pg.ViewBox.YAxis)

        # Remove all attached curves from the plot
        plot = self.plotWidget.getPlotItem()
        for curve in self.plotCurves:
            plot.removeItem(curve.plotDataItem)

        if self.legend is not None and self.legendEnabled:
            plot.removeItem(self.legend)

        # Create legend
        if self.legendEnabled:
            self.legend = pg.LegendItem((100, 60), offset=(70, 30))
            self.legend.setParentItem(self.plotWidget.getPlotItem())

        # Initialize all curves
        self.plotCurves = list()
        fields = self.dataFields['fields']
        names = self.dataFields['fieldNames']
        colorIndex = 0
        # Add for every data field one plot curve
        for i in range(0, len(fields)):
            curve = PlotCurve(self, fields[i], names[i])
            # Set color
            curve.setCurveColor(self.curveColors[colorIndex])
            # Add item to legend
            if self.legendEnabled:
                self.legend.addItem(curve.plotDataItem, names[i])
            # Set the max number of data points
            curve.setMaxDataSize(self._scopeLength/self._updateInterval)
            # Add curve to the plot
            self.plotWidget.addItem(curve.plotDataItem)
            # Save the curve for later
            self.plotCurves.append(curve)
            colorIndex += 1
            if colorIndex >= len(self.curveColors):
                colorIndex = 0

        # Create buttons for dis/enable individual curves
        self._createCurveButtons()

    def _createCurveButtons(self):
        # Remove all buttons from the layout if any
        for i in xrange(0, len(self.curveButtons)):
            b = self.curveButtonsLayout.takeAt(0)
            self.curveButtons.pop(0)
            b.widget().deleteLater()

        # Add buttons           
        i = 0
        for curve in self.plotCurves:
            button = QtGui.QPushButton(curve.dataName)
            button.setCheckable(True)
            button.setChecked(True)
            button.setStyleSheet("QPushButton {color: %s; font-weight: bold;}" % (self.curveColorsButton[i]))
            QtCore.QObject.connect(button, QtCore.SIGNAL('clicked()'),
                self.signalMapper,
                QtCore.SLOT("map()"))
            self.signalMapper.setMapping(button, i)
            self.curveButtons.append(button)
            self.curveButtonsLayout.addWidget(button)
            i = i + 1

    def _onCurveButtonToggled(self, nr):
        button = self.curveButtons[nr]
        curve = self.plotCurves[nr]

        if button.isChecked():
            # Button is checked, show the curve
            self.plotWidget.addItem(curve.plotDataItem)
        else:
            # Button is not checked, hide the curve
            self.plotWidget.removeItem(curve.plotDataItem)

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
                if not curve or not curve.yData or len(curve.yData) < 1:
                    continue
                    
                val = max(curve.yData)
                if val > max_yVal:
                    max_yVal = val

            # Find min y-value of all curves
            min_yVal = sys.maxint
            for curve in self.plotCurves:
                if not curve or not curve.yData or len(curve.yData) < 1:
                    continue

                val = min(curve.yData)
                if val < min_yVal:
                    min_yVal = val


            # Set the ranges for the x- and y-axis
            self.plotWidget.setRange(xRange=(self.plotCurves[0].xData[0],
                max(self._scopeLength, self.plotCurves[0].xData[-1])),
                yRange=(min_yVal, max_yVal))

        # Draw every curve
        for curve in self.plotCurves:
            if not curve:
                continue
            curve.drawCurve()

    def _on_vtol_object_received(self):
        """Updates the plot data. Called when the board status was updated."""
        if self._isRunning:
            timestamp = time.clock() - self._startTime
            for curve in self.plotCurves:
                curve.updateData(self.boardStatus, timestamp)

    def start(self):
        """
        Starts plotting.
        """
        self._initPlot()
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
        self._initPlot()
