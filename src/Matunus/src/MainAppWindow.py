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


# This is only needed for Python v2 but is harmless for Python v3.
import sip
sip.setapi('QVariant', 2)

try:
    from PyQt4 import QtGui
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)

import sys
import serial

from MainPage import MainPage
from ServoPage import ServoPage
from EnginePage import EnginePage
from ReceiverPage import ReceiverPage
from FlyModePage import FlyModePage
from serial_api import SerialAPI
from serial_api import CommandMessage, CommandType

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
        self.comPortsWindows = {'COM1' : 1, 'COM2' : 2, 'COM3' : 3, 'COM4' : 4,'COM5' : 5,
                         'COM6' : 6, 'COM7' : 7, 'COM8' : 8, 'COM9' : 9, 'COM10' : 10,
                         'COM11' : 11, 'COM12' : 12, 'COM13' : 13, 'COM14' : 14, 'COM15' : 15}
        self.comPortsLinux = {'/dev/ttys1'}
        self.serial = SerialAPI(5, 9600)

        self.createUi()
        self.createActions()
        self.createMenus()

        self.resize(700, 600)
        self.setWindowTitle("Matunus")
        self.statusBar().showMessage("Ready", 5000)

        self.updateUi()

    def createUi(self):
        self.mainLayout = QtGui.QVBoxLayout()

        self.topGroupBox = QtGui.QGroupBox("Connect to Controller")
        topLayout = QtGui.QHBoxLayout()
        self.bConnect = QtGui.QPushButton("Connect")
        self.bConnect.clicked.connect(self.connectToController)
        topLayout.addWidget(self.bConnect)
        self.bDisconnect = QtGui.QPushButton("Disconnect")
        self.bDisconnect.clicked.connect(self.disconnectFromController)
        topLayout.addWidget(self.bDisconnect)
        lStatus = QtGui.QLabel("Status: ")
        topLayout.addWidget(lStatus)
        self.lConnectionStatus = QtGui.QLabel("Here goes the status...")
        topLayout.addWidget(self.lConnectionStatus)
        topLayout.insertSpacing(2, 70)
        topLayout.insertStretch(5)
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

        self.mainContainer = QtGui.QTabWidget()
        self.mainPage = MainPage()
        self.mainContainer.addTab(self.mainPage, "Main")
        self.receiverPage = ReceiverPage()
        self.mainContainer.addTab(self.receiverPage, "Receiver")
        self.servoPage = ServoPage(self.serial)
        self.mainContainer.addTab(self.servoPage, "Servo")
        self.enginePage = EnginePage()
        self.mainContainer.addTab(self.enginePage, "Engine")
        self.flyModePage = FlyModePage()
        self.mainContainer.addTab(self.flyModePage, "Flight Mode")
        self.mainLayout.addWidget(self.mainContainer)

        widget = QtGui.QWidget()
        widget.setLayout(self.mainLayout)
        self.setCentralWidget(widget)

    def createMenus(self):
        self.fileMenu = self.menuBar().addMenu("&File")
        self.helpMenu = self.menuBar().addMenu("&Help")
        self.fileMenu.addAction(self.exitAction)

    def createActions(self):
        self.exitAction = QtGui.QAction("&Exit", self)
        self.exitAction.setShortcut('Ctrl+Q')
        self.exitAction.setStatusTip("Exit application")
        self.exitAction.triggered.connect(QtGui.qApp.quit)

    def updateUi(self):
        if self.serial.connected:
            self.lConnectionStatus.setText("<b>Connected</b>")
            self.bConnect.setEnabled(False)
            self.bDisconnect.setEnabled(True)
        else:
            self.lConnectionStatus.setText("<b>Disconnected</b>")
            self.bConnect.setEnabled(True)
            self.bDisconnect.setEnabled(False)
            self.mainPage.setSystemStatus("Disconnected")

    def connectToController(self):
        print "Connecting to controller..."
        self.statusBar().showMessage("Connecting to controller...", 2000)

        self.serial.connect()
        #self.serial.writeCommand(CommandMessage(CommandType.LEDSON))
        self.statusBar().showMessage("Connected!", 2000)
        self.updateUi()

    def disconnectFromController(self):
        print "Disconnecting from controller..."

        if self.serial is not None and self.serial.connected:
            #self.serial.writeCommand(CommandMessage(CommandType.LEDSOFF))
            self.serial.disconnect()
            self.statusBar().showMessage("Disconnected!", 2000)
            self.updateUi()

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
