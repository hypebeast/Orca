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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

__author__ = 'Sebastian Ruml'


import sys

try:
    from PyQt4 import QtGui
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)


import defs
from ScopeWidget import ScopeWidget
from logger import Logger
from ScopeExportDialog import ScopeExportDialog


class ScopePage(QtGui.QWidget):
    """
    This page holds all scope widgetes.
    """
    def __init__(self, boardController):
        super(ScopePage, self).__init__()

        if boardController is None:
            raise "Error!"

        self._boardController = boardController
        self._scopes = list()
        self._app_defs = defs.AppDefs()
        self._logger = Logger()
        self._isRunning = False

        # Available data fields
        self.dataObjects = list()
        self.dataObjects.append({'groupName': "Input Channels",
                                'fields': ("inputChannel1", "inputChannel2", "inputChannel3", "inputChannel4", "inputChannel5", "inputChannel6", "inputChannel7"),
                                'fieldNames': ("Input Channel 1", "Input Channel 2", "Input Channel 3", "Input Channel 4", "Input Channel 5", "Input Channel 6", "Input Channel 7")})
        self.dataObjects.append({'groupName': "Output Channels",
                                'fields': ("outputChannel1", "outputChannel2", "outputChannel3", "outputChannel4", "outputChannel5", "outputChannel6"),
                                'fieldNames': ("Output Channel 1", "Output Channel 2", "Output Channel 3", "Output Channel 4", "Output Channel 5", "Output Channel 6")})
        self.dataObjects.append({'groupName': "Gyro X, Y, Z",
                                'fields': ("gyroX", "gyroY", "gyroZ"),
                                'fieldNames': ("Gyro X", "Gyro Y", "Gyro Z")})
        self.dataObjects.append({'groupName': "Acceleration X, Y, Z",
                                'fields': ("accelerationX", "accelerationY", "accelerationZ"),
                                'fieldNames': ("Acceleration X", "Acceleration Y", "Acceleration Z")})
        self.dataObjects.append({'groupName': "Kalman Reference/Output Value",
                                'fields': ("kalmanOutputRoll", "kalmanReferenceValueRoll"),
                                'fieldNames': ("Output Roll", "Reference Roll")})
        self.dataObjects.append({'groupName': "Set Value Angle",
                                'fields': ("setValueRollAngle",),
                                'fieldNames': ("Roll",)})
        self.dataObjects.append({'groupName': "Actuating Variable PID",
                                'fields': ("actuatingVariablePidRoll",),
                                'fieldNames': ("Roll",)})

        # Create the UI
        self._createUi()

    def _createUi(self):
        """Initializes the UI."""
        mainLayout = QtGui.QVBoxLayout()
        
        # Palette for the headers
        palette =  QtGui.QPalette()
        palette.setColor(QtGui.QPalette.Background, QtGui.QColor(91, 91, 91))
        palette.setColor(QtGui.QPalette.WindowText, QtGui.QColor(255, 0, 0))

        # Label for the headers
        label1 = QtGui.QLabel("Data Fields")
        label1.setStyleSheet("QLabel { font: bold; color: #FFFFFF; }")
        label2 = QtGui.QLabel("Data Fields")
        label2.setStyleSheet("QLabel { font: bold; color: #FFFFFF; }")

        # Header 1
        header = QtGui.QFrame()
        header.setPalette(palette)
        header.setMinimumHeight(30)
        header.setAutoFillBackground(True)
        header.setFrameStyle(QtGui.QFrame.StyledPanel | QtGui.QFrame.Raised)
        header.setLineWidth(2)
        layout = QtGui.QHBoxLayout()
        header.setLayout(layout)
        layout.addWidget(label1)
        self.cbdataObjects1 = QtGui.QComboBox()
        for item in self.dataObjects:
            self.cbdataObjects1.addItem(item['groupName'])
        self.cbdataObjects1.setCurrentIndex(0)
        layout.addWidget(self.cbdataObjects1)
        layout.addStretch()
        mainLayout.addWidget(header)

        # Scope 1
        self.scope1 = ScopeWidget(self._boardController, self.dataObjects[0])
        mainLayout.addWidget(self.scope1)
        self._scopes.append(self.scope1)

         # Header 2
        header = QtGui.QFrame()
        header.setPalette(palette)
        header.setMinimumHeight(30)
        header.setAutoFillBackground(True)
        header.setFrameStyle(QtGui.QFrame.StyledPanel | QtGui.QFrame.Raised)
        header.setLineWidth(2)
        layout = QtGui.QHBoxLayout()
        header.setLayout(layout)
        layout.addWidget(label2)
        self.cbdataObjects2 = QtGui.QComboBox()
        for item in self.dataObjects:
            self.cbdataObjects2.addItem(item['groupName'])
        self.cbdataObjects2.setCurrentIndex(0)
        layout.addWidget(self.cbdataObjects2)
        layout.addStretch()
        mainLayout.addWidget(header)
        
        # Scope 2
        self.scope2 = ScopeWidget(self._boardController, self.dataObjects[0])
        mainLayout.addWidget(self.scope2)
        self._scopes.append(self.scope2)

        # Control box
        groupBox = QtGui.QGroupBox("Scope Control")
        groupBox.setMinimumHeight(40)
        layout = QtGui.QHBoxLayout()
        groupBox.setLayout(layout)
        self.startButton = QtGui.QPushButton("Start")
        self.startButton.clicked.connect(self._startClicked)
        layout.addWidget(self.startButton)
        self.stopButton = QtGui.QPushButton("Stop")
        self.stopButton.setEnabled(False)
        self.stopButton.clicked.connect(self._stopClicked)
        layout.addWidget(self.stopButton)
        layout.addStretch()
        self.exportButton = QtGui.QPushButton("Export...")
        self.exportButton.setEnabled(False)
        self.exportButton.clicked.connect(self._onExportButtonClicked)
        layout.addWidget(self.exportButton)
        mainLayout.addWidget(groupBox)

        self.cbdataObjects1.currentIndexChanged.connect(self._cbdataObjects1IndexChanged)
        self.cbdataObjects2.currentIndexChanged.connect(self._cbdataObjects2IndexChanged)

        self.setLayout(mainLayout)

    def start(self):
        """Starts scoping."""
        if self._isRunning:
            return

        self.scope1.start()
        self.scope2.start()
        self.startButton.setEnabled(False)
        self.stopButton.setEnabled(True)
        self.exportButton.setEnabled(False)
        self.cbdataObjects1.setEnabled(False)
        self.cbdataObjects2.setEnabled(False)
        self._isRunning = True

    def stop(self):
        """Stops scoping."""
        if not self._isRunning:
            return

        self.scope1.stop()
        self.scope2.stop()
        self.startButton.setEnabled(True)
        self.stopButton.setEnabled(False)
        self.exportButton.setEnabled(True)
        self.cbdataObjects1.setEnabled(True)
        self.cbdataObjects2.setEnabled(True)
        self._isRunning = False

    def _exportToCSV(self, scope, fileName):
        """Exports the current scope values to a CSV file."""
        if fileName is None or scope is None:
            return

        f = open(fileName, 'w')

        with f:
            dataFields = None
            curves = None
            if scope == 1:
                dataFields = self.scope1.dataFields
                curves = self.scope1.plotCurves
            else:
                dataFields = self.scope2.dataFields
                curves = self.scope2.plotCurves

            dataNames = "Timestamp,"
            dataNames += ','.join(dataFields['fieldNames'])
            f.write(dataNames+'\n')
            dataLength = len(curves[0].xData)
            for i in range(0, dataLength):
                line = str(curves[0].xData[i])
                line += ','
                for curve in curves:
                    line += str(curve.yData[i])
                    line += ','
                line = line[:-1]
                line += '\n'
                f.write(line)

            f.close()

    def _startClicked(self):
        self.start()

    def _stopClicked(self):
        self.stop()

    def _cbdataObjects1IndexChanged(self):
        self.scope1.setDataFields(self.dataObjects[self.cbdataObjects1.currentIndex()])

    def _cbdataObjects2IndexChanged(self):
        self.scope2.setDataFields(self.dataObjects[self.cbdataObjects2.currentIndex()])

    def _onExportButtonClicked(self):
        dialog = ScopeExportDialog(self)
        res = dialog.exec_()
        if res == QtGui.QDialog.Accepted:
            self._exportToCSV(dialog.selectedScope, dialog.fileName)
