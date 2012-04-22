#!/usr/bin/env python

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

# This is only needed for Python v2 but is harmless for Python v3.
import os
import sip
sip.setapi('QVariant', 2)

try:
    from PyQt4 import QtGui
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)

import sys

from MainPage import MainPage
from ServoPage import ServoPage
from EnginePage import EnginePage
from ReceiverPage import ReceiverPage
from ConfigurationPage import ConfigurationPage
from FlightControlPage import FlightControlPage
from GpsPage import GpsPage
from AttitudePage import AttitudePage
from serial_api import SerialAPI

class SelectControllerDialog(QtGui.QDialog):
    def __init__(self):
        super(SelectControllerDialog, self).__init__()

        layout = QtGui.QVBoxLayout()
        self.cbController = QtGui.QComboBox()
        self.cbController.addItem("Test Controller")
        layout.addWidget(self.cbController)
        hBox = QtGui.QHBoxLayout()
        bOk = QtGui.QPushButton("&Ok")
        bOk.clicked.connect(self.okClicked)
        hBox.addWidget(bOk)
        bCancel = QtGui.QPushButton("&Cancel")
        bCancel.clicked.connect(self.cancelClicked)
        hBox.addWidget(bCancel)
        hBox.insertStretch(0)
        layout.addItem(hBox)
        layout.addStretch()
        self.setLayout(layout)

    def okClicked(self):
        pass

    def cancelClicked(self):
        pass


class MainAppWindow(QtGui.QMainWindow):
    def __init__(self):
        super(MainAppWindow, self).__init__()

        # Serial connection
        if os.name == 'nt':
            self.comPortsWindows = {'COM1' : 1, 'COM2' : 2, 'COM3' : 3, 'COM4' : 4,'COM5' : 5,
                                    'COM6' : 6, 'COM7' : 7, 'COM8' : 8, 'COM9' : 9, 'COM10' : 10,
                                    'COM11' : 11, 'COM12' : 12, 'COM13' : 13, 'COM14' : 14, 'COM15' : 15}
            self.comPortsLinux = {'/dev/ttys1'}
            self.serial = SerialAPI(5, 9600)
        elif os.name == 'posix':
            pass
        else:
            raise NotImplementedError("Sorry no implementation for your platform (%s) available." % sys.platform)

        self.createUi()
        self.createActions()
        self.createMenus()

        self.resize(700, 700)
        self.setWindowTitle("Matunus")
        self.statusBar().showMessage("Ready", 3000)

        self.updateUi()

    def createUi(self):
        self.mainLayout = QtGui.QVBoxLayout()

        self.topGroupBox = QtGui.QGroupBox("Control")
        topLayout = QtGui.QHBoxLayout()
        self.bConnect = QtGui.QPushButton("Connect")
        self.bConnect.clicked.connect(self.connectToController)
        topLayout.addWidget(self.bConnect)
        topLayout.addSpacing(15)
        self.bStartStop = QtGui.QPushButton("Start")
        topLayout.addWidget(self.bStartStop)
        topLayout.addStretch()
        lComPort = QtGui.QLabel("Com Port: ")
        topLayout.addWidget(lComPort)
        self.cbComPort = QtGui.QComboBox()
        comNames = self.comPortsWindows.keys()
        comNames.sort()
        for name in comNames:
            self.cbComPort.addItem(name)
        #self.bSelectController = QtGui.QPushButton("Select Controller...")
        #self.bSelectController.clicked.connect(self.selectControllerClicked)
        #topLayout.addWidget(self.bSelectController)
        topLayout.addWidget(self.cbComPort)
        self.topGroupBox.setLayout(topLayout)
        self.mainLayout.addWidget(self.topGroupBox)

        # Status Bar
        self.lStatusBarLabel = QtGui.QLabel()
        self.statusBar().addPermanentWidget(self.lStatusBarLabel, 0)

        # Pages
        self.pages = []
        self.mainContainer = QtGui.QTabWidget()
        self.mainPage = MainPage()
        self.pages.append(self.mainPage)
        self.mainContainer.addTab(self.mainPage, "Main")
        self.receiverPage = ReceiverPage()
        self.pages.append(self.receiverPage)
        self.mainContainer.addTab(self.receiverPage, "Receiver")
        self.servoPage = ServoPage(self.serial)
        self.pages.append(self.servoPage)
        self.mainContainer.addTab(self.servoPage, "Servo")
        self.enginePage = EnginePage()
        self.pages.append(self.enginePage)
        self.mainContainer.addTab(self.enginePage, "Engine")
        self.attitudePage = AttitudePage()
        self.pages.append(self.attitudePage)
        self.mainContainer.addTab(self.attitudePage, "Attitude")
        self.gpsPage = GpsPage()
        self.pages.append(self.gpsPage)
        self.mainContainer.addTab(self.gpsPage, "GPS")
        self.flightControlPage = FlightControlPage()
        self.pages.append(self.flightControlPage)
        self.mainContainer.addTab(self.flightControlPage, "Flight Control")
        self.configurationPage = ConfigurationPage()
        self.pages.append(self.configurationPage)
        self.mainContainer.addTab(self.configurationPage, "Configuration")
        self.mainLayout.addWidget(self.mainContainer)

        widget = QtGui.QWidget()
        widget.setLayout(self.mainLayout)
        self.setCentralWidget(widget)

    def createMenus(self):
        self.fileMenu = self.menuBar().addMenu("&File")
        self.settingsMenu = self.menuBar().addMenu("&Settings")
        self.helpMenu = self.menuBar().addMenu("&Help")
        self.fileMenu.addAction(self.exitAction)
        self.settingsMenu.addAction(self.settingsAction)

    def createActions(self):
        self.exitAction = QtGui.QAction("&Exit", self)
        self.exitAction.setShortcut('Ctrl+Q')
        self.exitAction.setStatusTip("Exit application")
        self.exitAction.triggered.connect(QtGui.qApp.quit)
        self.settingsAction = QtGui.QAction("&Settings", self)
        self.settingsAction.setShortcut('Ctrl+S')
        self.settingsAction.setStatusTip("Settings")

    def updateUi(self):
        if self.serial.connected:
            self.lStatusBarLabel.setText("<b>Connected</b>")
        else:
            self.lStatusBarLabel.setText("<b>Disconnected</b>")

    def connectToController(self):
        if not self.serial.connected:
            self.statusBar().showMessage("Connecting to controller...", 2000)

            self.serial.connect()
            #self.serial.writeCommand(CommandMessage(CommandType.LEDSON))
            self.statusBar().showMessage("Connected!", 2000)
            self.lStatusBarLabel.setText("<b>Connected</b>")
            self.updateUi()
            self.mainPage.setSystemStatus("Ready")
            self.mainPage.setSystemMessage("Connected to the controller")
            self.bConnect.setText("<b>Disconnect</b>")
        elif self.serial.connected:
            #self.serial.writeCommand(CommandMessage(CommandType.LEDSOFF))
            self.serial.disconnect()
            self.statusBar().showMessage("Disconnected!", 2000)
            self.lStatusBarLabel.setText("<b>Connected</b>")
            self.updateUi()
            self.mainPage.setSystemStatus("Off")
            self.mainPage.setSystemMessage("Disconnected from the controller")
            self.bConnect.setText("<b>Connect</b>")

    def selectControllerClicked(self):
        print "Select controller clicked"
        dialog = SelectControllerDialog()
        dialog.setModal(True)
        dialog.show()


class App():
    def __init__(self, options= None, args=None):
        app = QtGui.QApplication(sys.argv)
        mainWin = MainAppWindow()
        mainWin.show()
        sys.exit(app.exec_())
