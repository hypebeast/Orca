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


import os

from PyQt4 import QtGui, QtCore

#try:
#    from PyQt4 import QtGui, QtCore
#except ImportError:
#    print "No PyQt found!"
#    import sys
#    sys.exit(2)

from OutputPage import OutputPage
from ReceiverPage import ReceiverPage
from AttitudePage import AttitudePage
from GpsPage import GpsPage
from StabilizationPage import StabilizationPage

import defs


class ConfigurationPage(QtGui.QWidget):
    def __init__(self, controllerManager=None):
        super(ConfigurationPage, self).__init__()

        if controllerManager is None:
            raise Exception

        self.controllerManager = controllerManager
        self.app_defs = defs.AppDefs()
        self.createUi()

    def createUi(self):
        self.mainLayout = QtGui.QHBoxLayout()
        #self.mainLayout.setMargin(20)

        # Left side panel (navigation bar)
        model = QtGui.QStandardItemModel()
        icon = QtGui.QIcon(os.path.join(self.app_defs.IconsPath, "Transmitter.png"))
        item = QtGui.QStandardItem(icon, "Input")
        model.appendRow(item)
        icon = QtGui.QIcon(os.path.join(self.app_defs.IconsPath, "Servo.png"))
        item = QtGui.QStandardItem(icon, "Output")
        model.appendRow(item)
        icon = QtGui.QIcon(os.path.join(self.app_defs.IconsPath, "AHRS-v1.3.png"))
        item = QtGui.QStandardItem(icon, "Attitude")
        model.appendRow(item)
        icon = QtGui.QIcon(os.path.join(self.app_defs.IconsPath, "gyroscope.png"))
        item = QtGui.QStandardItem(icon, "Stabilization")
        model.appendRow(item)
        icon = QtGui.QIcon(os.path.join(self.app_defs.IconsPath, "Satellite.png"))
        item = QtGui.QStandardItem(icon, "GPS")
        model.appendRow(item)

        self.navBar = QtGui.QListView()
        self.navBar.setModel(model)
        self.navBar.setMinimumWidth(50)
        self.navBar.setMaximumWidth(80)
        self.navBar.setViewMode(QtGui.QListView.IconMode)
        self.navBar.setSpacing(5)
        self.navBar.setMovement(QtGui.QListView.Static)
        self.navBar.setIconSize(QtCore.QSize(60,60))
        self.navBar.clicked.connect(self.navBarItemClicked)
        self.mainLayout.addWidget(self.navBar)

        # Main Container and all pages
        self.mainContainer = QtGui.QStackedLayout()

        page = ReceiverPage()
        self.mainContainer.addWidget(page)

        page = OutputPage(self.controllerManager)
        self.mainContainer.addWidget(page)

        page = AttitudePage()
        self.mainContainer.addWidget(page)

        page = StabilizationPage()
        self.mainContainer.addWidget(page)

        page = GpsPage()
        self.mainContainer.addWidget(page)

        self.mainContainer.setCurrentIndex(0)
        self.mainLayout.addLayout(self.mainContainer, 1)
        self.setLayout(self.mainLayout)

        # Set thebackground color
        #palette = QtGui.QPalette(self.palette())
        #palette.setColor(QtGui.QPalette.Background, QtCore.Qt.black)
        #self.setPalette(palette)

    def navBarItemClicked(self, index):
        self.mainContainer.setCurrentIndex(index.row())