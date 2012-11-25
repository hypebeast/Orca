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

try:
    from PyQt4 import QtGui, QtCore
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)

import os
import sys

from WelcomePage import WelcomePage
from InfoPanel import InfoPanel
from ConfigurationPage import ConfigurationPage
from ScopePage import ScopePage
from FlightDisplay import FlightDisplay
from SerialConnection import SerialError
from SettingsDialog import SettingsDialog
from ControllerManager import ControllerManager
from logger import Logger
import defs
import utils


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
    """
    Main window of the application.
    """
    def __init__(self, options=None, args=None):
        super(MainAppWindow, self).__init__()

        # Save options and arguments for later
        self.options = options
        self.args = args

        # Initialize logger
        self._logger = Logger()
        if options.debug:
            self._logger.set_level("DEBUG")
        self._logger.info("Starting Matunus...")

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

            # Find available COM ports
            self.availableComPorts = list()
            ports = utils.enumerate_serial_ports()
            for i, port in enumerate(ports):
                self.availableComPorts.append(port)
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
            raise NotImplementedError("Sorry, there is no implementation for your platform (%s) available." % sys.platform)

        # App definitions
        self.appDefs = defs.AppDefs()

        # TODO: Load configuration

        # Provides methods for interacting with the flight controller
        self.controllerManager = ControllerManager()

        # Create UI
        self._createUi()
        self._createActions()
        self._createMenus()
        self._connectToSignals()

        self.resize(1200, 850)
        self._center()
        self.setWindowTitle("Matunus")
        self.statusBar().showMessage("Ready", 3000)

        self.updateUi()

        self._logger.info("Startup done!")

    def _createUi(self):
        # Set app icon
        self.setWindowIcon(QtGui.QIcon(os.path.join(self.appDefs.IconsPath, "app.png")))

        # Main layout
        self.mainLayout = QtGui.QHBoxLayout()

        # Info panel
        self.infoPanel = InfoPanel(self.availableComPorts, self.controllerManager)
        self.infoPanel.bConnect.clicked.connect(self.connectToController)
        self.mainLayout.addWidget(self.infoPanel)

        # Layout for the page area
        self.pageAreaLayout = QtGui.QVBoxLayout()
        self.mainLayout.addLayout(self.pageAreaLayout)

        # Status Bar
        self.lStatusBarLabel = QtGui.QLabel()
        self.statusBar().addPermanentWidget(self.lStatusBarLabel, 0)

        # Pages
        self.pages = []
        self.mainContainer = QtGui.QTabWidget()
        self.mainContainer.setTabPosition(QtGui.QTabWidget.South)
        self.mainContainer.setTabShape(QtGui.QTabWidget.Rounded)

        # Welcome page
        self.welcomePage = WelcomePage()
        self.pages.append(self.welcomePage)
        icon = QtGui.QIcon(os.path.join(self.appDefs.IconsPath, "config.png"))
        self.mainContainer.addTab(self.welcomePage, icon, "Home")

        # Flight display page
        self.flightDisplay = FlightDisplay()
        self.pages.append(self.flightDisplay)
        icon = QtGui.QIcon(os.path.join(self.appDefs.IconsPath, "flight_display.png"))
        self.mainContainer.addTab(self.flightDisplay, icon, "Flight Display")

        #self.flightControlPage = FlightControlPage()
        #self.pages.append(self.flightControlPage)
        #icon = QtGui.QIcon(os.path.join(self.appDefs.IconsPath, "joystick.png"))
        #self.mainContainer.addTab(self.flightControlPage, icon, "Flight Control")
        
        # Configuration page
        self.configurationPage = ConfigurationPage(self.controllerManager)
        self.pages.append(self.configurationPage)
        icon = QtGui.QIcon(os.path.join(self.appDefs.IconsPath, "config.png"))
        self.mainContainer.addTab(self.configurationPage, icon, "Configuration")

        # Scope page
        self.scopePage = ScopePage(self.controllerManager)
        self.pages.append(self.scopePage)
        icon = QtGui.QIcon(os.path.join(self.appDefs.IconsPath, "scopes.png"))
        self.mainContainer.addTab(self.scopePage, icon, "Scopes")

        # Add the main containter to the main layout
        self.pageAreaLayout.addWidget(self.mainContainer)

        widget = QtGui.QWidget()
        widget.setLayout(self.mainLayout)
        self.setCentralWidget(widget)

    def _center(self):
        qr = self.frameGeometry()
        cp = QtGui.QDesktopWidget().availableGeometry().center()
        qr.moveCenter(cp)
        self.move(qr.topLeft())

    def _createMenus(self):
        self.fileMenu = self.menuBar().addMenu("&File")
        self.settingsMenu = self.menuBar().addMenu("&Settings")
        self.helpMenu = self.menuBar().addMenu("&Help")
        self.fileMenu.addAction(self.exitAction)
        self.settingsMenu.addAction(self.settingsAction)

    def _createActions(self):
        self.exitAction = QtGui.QAction("&Exit", self)
        self.exitAction.setShortcut('Ctrl+Q')
        self.exitAction.setStatusTip("Exit application")
        self.exitAction.triggered.connect(QtGui.qApp.quit)
        self.settingsAction = QtGui.QAction("&Settings", self)
        self.settingsAction.setShortcut('Ctrl+S')
        self.settingsAction.setStatusTip("Settings")
        self.settingsAction.triggered.connect(self.showSettingsDialog)

    def _connectToSignals(self):
        pass

    def updateUi(self):
        if self.controllerManager.connected():
            self.lStatusBarLabel.setText("<b>Connected</b>")
        else:
            self.lStatusBarLabel.setText("<b>Disconnected</b>")

    def connectToController(self):
        if not self.controllerManager.connected():
            self.statusBar().showMessage("Connecting to controller...", 2000)

            try:    
                # Connect
                self.controllerManager.connect()

                self.statusBar().showMessage("Connected!", 2000)
                self.lStatusBarLabel.setText("<b>Connected</b>")
                self.updateUi()
                self.infoPanel.setSystemStatus("Ready")
                self.infoPanel.setSystemMessage("Connected to the controller")
            except SerialError as ex:
                msgBox = QtGui.QMessageBox()
                msgBox.setWindowTitle("Serial Error")
                msgBox.setText("Can't connect to the controller")
                msgBox.setInformativeText(ex.msg)
                msgBox.setStandardButtons(QtGui.QMessageBox.Ok)
                msgBox.setIcon(QtGui.QMessageBox.Warning)
                msgBox.exec_()
        elif self.controllerManager.connected():
            # Disconnect
            self.controllerManager.disconnect()

            self.statusBar().showMessage("Disconnected!", 2000)
            self.lStatusBarLabel.setText("<b>Connected</b>")
            self.updateUi()
            self.infoPanel.setSystemStatus("Not Connected")
            self.infoPanel.setSystemMessage("Disconnected from the controller")

    def selectControllerClicked(self):
        print "Select controller clicked"
        dialog = SelectControllerDialog()
        dialog.setModal(True)
        dialog.show()

    def comPortChanged(self, index):
        #self.controllerManager.set_serial_port(self.comPorts[index]["value"])
        self.controllerManager.set_serial_port(self.availableComPorts[index])

    def showSettingsDialog(self):
        dlg = SettingsDialog()
        dlg.exec_()


class App():
    def __init__(self, options= None, args=None):
        """
        Run the app.
        """
        app_defs = defs.AppDefs()

        app = QtGui.QApplication(sys.argv)
        qss = QtCore.QFile(os.path.join(app_defs.StylesheetPath, "default_windows.qss"))
        qss.open(QtCore.QFile.ReadOnly)
        app.setStyleSheet(QtCore.QString(qss.readAll()))
        qss.close()
        mainWin = MainAppWindow(options, args)
        mainWin.show()
        sys.exit(app.exec_())
