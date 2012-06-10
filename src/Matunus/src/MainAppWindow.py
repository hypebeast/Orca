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
from threading import Thread
import time

from MainPage import MainPage
from ServoPage import ServoPage
from EnginePage import EnginePage
from ReceiverPage import ReceiverPage
from ConfigurationPage import ConfigurationPage
from FlightControlPage import FlightControlPage
from GpsPage import GpsPage
from AttitudePage import AttitudePage
from serial_api import SerialAPI
from serial_api import SerialError
from SettingsDialog import SettingsDialog
from BoardStatus import BoardStatus
from logger import Logger


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
            self.comPorts = [{'name': u"COM1", "value": 0},
                             {'name': u"COM2", "value": 1},
                             {'name': u"COM3", "value": 2},
                             {'name': u"COM4", "value": 3},
                             {'name': u"COM5", "value": 4},
                             {'name': u"COM6", "value": 5},
                             {'name': u"COM7", "value": 6},
                             {'name': u"COM8", "value": 7},
                             {'name': u"COM9", "value": 8},
                             {'name': u"COM10", "value": 9},
                             {'name': u"COM11", "value": 10},
                             {'name': u"COM12", "value": 11},
                             {'name': u"COM13", "value": 12},
                             {'name': u"COM14", "value": 13},
                             {'name': u"COM15", "value": 14}]
        elif os.name == 'posix':
            self.comPorts = [{"name": u"/dev/ttys0", "value": 0},
                             {"name": u"/dev/ttys1", "value": 1},
                             {"name": u"/dev/ttys2", "value": 2},
                             {"name": u"/dev/ttys3", "value": 3},
                             {"name": u"/dev/ttys4", "value": 4},
                             {"name": u"/dev/ttys5", "value": 5},
                             {"name": u"/dev/ttys6", "value": 6},
                             {"name": u"/dev/ttys7", "value": 7},
                             {"name": u"/dev/ttys8", "value": 8},
                             {"name": u"/dev/ttys9", "value": 9},
                             {"name": u"/dev/ttys10", "value": 10},
                             ]
        else:
            raise NotImplementedError("Sorry no implementation for your platform (%s) available." % sys.platform)

        # TODO: Loading configuration
        
        # Initialize logger
        self._logger = Logger()
        self._logger.info("Starting Matunus...")

        # Board status data
        self.boardStatus = BoardStatus()
        
        # Serial connection
        self.serial_connection = SerialAPI()
        self.serial_connection.set_port(0)

        # Create UI
        self.createUi()
        self.createActions()
        self.createMenus()
        self.connectToSignals()

        self.resize(750, 700)
        self.setWindowTitle("Matunus")
        self.statusBar().showMessage("Ready", 3000)

        # Add serial reader thread
        self.serialReaderThread = Thread(target=self.updateBoardStatus)
        self.statusReaderAlive = False

        self.updateUi()

        self._logger.info("Startup done!")

    def createUi(self):
        self.mainLayout = QtGui.QVBoxLayout()

        self.topGroupBox = QtGui.QGroupBox("Control")
        topLayout = QtGui.QHBoxLayout()
        self.bConnect = QtGui.QPushButton("Connect")
        self.bConnect.clicked.connect(self.connectToController)
        topLayout.addWidget(self.bConnect)
        topLayout.addSpacing(15)
        topLayout.addStretch()
        lComPort = QtGui.QLabel("Com Port: ")
        topLayout.addWidget(lComPort)
        self.cbComPort = QtGui.QComboBox()
        for comPort in self.comPorts:
            self.cbComPort.addItem(comPort["name"])
        self.cbComPort.currentIndexChanged.connect(self.comPortChanged)
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
        self.flightControlPage = FlightControlPage()
        self.pages.append(self.flightControlPage)
        self.mainContainer.addTab(self.flightControlPage, "Flight Control")
        self.enginePage = EnginePage(self.serial_connection)
        self.pages.append(self.enginePage)
        self.mainContainer.addTab(self.enginePage, "Engine")
        self.servoPage = ServoPage(self.serial_connection)
        self.pages.append(self.servoPage)
        self.mainContainer.addTab(self.servoPage, "Servo")
        self.receiverPage = ReceiverPage()
        self.pages.append(self.receiverPage)
        self.mainContainer.addTab(self.receiverPage, "Receiver")
        self.attitudePage = AttitudePage()
        self.pages.append(self.attitudePage)
        self.mainContainer.addTab(self.attitudePage, "Attitude")
        self.gpsPage = GpsPage()
        self.pages.append(self.gpsPage)
        self.mainContainer.addTab(self.gpsPage, "GPS")
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
        self.settingsAction.triggered.connect(self.showSettingsDialog)

    def connectToSignals(self):
        pass

    def updateUi(self):
        if self.serial_connection.connected:
            self.lStatusBarLabel.setText("<b>Connected</b>")
            self.cbComPort.setEnabled(False)
        else:
            self.lStatusBarLabel.setText("<b>Disconnected</b>")
            self.cbComPort.setEnabled(True)

    def connectToController(self):
        if not self.serial_connection.connected:
            self.statusBar().showMessage("Connecting to controller...", 2000)

            try:    
                self.serial_connection.connect()

                # Start reading board status
                self.startStatusReader()

                #self.serial_connection.writeCommand(CommandMessage(CommandType.LEDSON))
                self.statusBar().showMessage("Connected!", 2000)
                self.lStatusBarLabel.setText("<b>Connected</b>")
                self.updateUi()
                self.mainPage.setSystemStatus("Ready")
                self.mainPage.setSystemMessage("Connected to the controller")
                self.bConnect.setText("Disconnect")
            except SerialError as ex:
                msgBox = QtGui.QMessageBox()
                msgBox.setWindowTitle("Serial Error")
                msgBox.setText("Can't connect to the controller")
                msgBox.setInformativeText(ex.msg)
                msgBox.setStandardButtons(QtGui.QMessageBox.Ok)
                msgBox.setIcon(QtGui.QMessageBox.Warning)
                msgBox.exec_()
            
        elif self.serial_connection.connected:
            # Stop reading board status
            self.stopStatusReader()

            # Disconnect
            self.serial_connection.disconnect()

            self.statusBar().showMessage("Disconnected!", 2000)
            self.lStatusBarLabel.setText("<b>Connected</b>")
            self.updateUi()
            self.mainPage.setSystemStatus("Not Connected")
            self.mainPage.setSystemMessage("Disconnected from the controller")
            self.bConnect.setText("Connect")

    def selectControllerClicked(self):
        print "Select controller clicked"
        dialog = SelectControllerDialog()
        dialog.setModal(True)
        dialog.show()

    def comPortChanged(self, index):
        self.serial_connection.set_port(self.comPorts[index]["value"])

    def showSettingsDialog(self):
        dlg = SettingsDialog()
        dlg.exec_()

    def startStatusReader(self):
        self.statusReaderAlive = True
        self.serialReaderThread = Thread(target=self.updateBoardStatus)
        self.serialReaderThread.setDaemon(True)
        self.serialReaderThread.start()

    def stopStatusReader(self):
        self.statusReaderAlive = False
        self.serialReaderThread.join()

    def updateBoardStatus(self):
        while self.statusReaderAlive:
            self.boardStatus = self.serial_connection.readStatus()
            time.sleep(5)


class App():
    def __init__(self, options= None, args=None):
        app = QtGui.QApplication(sys.argv)
        mainWin = MainAppWindow()
        mainWin.show()
        sys.exit(app.exec_())
