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

from vtolobjects import vtolobjectmanager
from logger import Logger
import defs
import random


class SystemPage(QtGui.QWidget):
    def __init__(self, fmu):
        super(SystemPage, self).__init__()

        if not fmu:
            raise Exception

        self._dataTreeItems = list()
        self._settingsTreeItems = list()

        self._fmu = fmu
        self._fmu.vtol_object_received.connect(self._onObjectReceived)
        self._vtolo_manager = vtolobjectmanager.VTOLObjectManager()
        self._app_defs = defs.AppDefs()
        self._logger = Logger()
        self._createUi()

        self._addDataObjects()
        self._addSettingObjects()

    def _createUi(self):
        mainLayout = QtGui.QVBoxLayout()

        # Toolbar
        hLayout = QtGui.QHBoxLayout()
        testButton = QtGui.QPushButton("Test")
        hLayout.addWidget(testButton)
        hLayout.addStretch()
        #mainLayout.addLayout(hLayout)

        # Tree View
        self._tree = QtGui.QTreeWidget()
        self._tree.setColumnCount(3)
        self._tree.setColumnWidth(0, 250)
        self._tree.setColumnWidth(1, 200)
        self._tree.setHeaderLabels(QtCore.QStringList(["Property", "Value", "Unit", "Description"]))
        mainLayout.addWidget(self._tree)

        self.setLayout(mainLayout)

    def _addDataObjects(self):
        data = QtGui.QTreeWidgetItem(self._tree)
        data.setText(0, "Data Objects")
        dataObjects = self._vtolo_manager.getDataObjects()
        for obj in dataObjects:
            item = QtGui.QTreeWidgetItem(data)
            item.setText(0, obj.getName())
            item.setText(3, obj.getDescription())
            for field in obj.getFields():
                fieldItem = QtGui.QTreeWidgetItem(item)
                fieldItem.setText(0, field.name)
                fieldItem.setText(1, str(field.value))
                fieldItem.setText(2, field.units)

            self._dataTreeItems.append(item)

    def _addSettingObjects(self):
        settings = QtGui.QTreeWidgetItem(self._tree)
        settings.setText(0, "Setting Objects")
        settingObjects = self._vtolo_manager.getSettingsObjects()
        for obj in settingObjects:
            print obj
            item = QtGui.QTreeWidgetItem(settings)
            item.setText(0, obj.getName())
            item.setText(3, obj.getDescription())
            for field in obj.getFields():
                fieldItem = QtGui.QTreeWidgetItem(item)
                fieldItem.setText(0, field.name)
                fieldItem.setText(1, str(field.value))
                fieldItem.setText(2, field.units)

            self._settingsTreeItems.append(item)

    def _onObjectReceived(self, obj):
        pass
        # dataFields = self._fmu.boardStatus.dataFields

        # for field in dataFields:
        #     for tItem in self._dataTreeItems:
        #         if tItem.text(0) == field.name:
        #             tItem.setText(1, str(field.value))
        #             break

    # def _onBoardSettingsUpdated(self):
    #     settingsField = self._fmu.fmuSettings.dataFields

    #     for field in settingsField:
    #         for tItem in self._settingsTreeItems:
    #             if tItem.text(0) == field.name:
    #                 tItem.setText(1, str(field.value))
    #                 break

    def _addTestData(self):
        """Adds some test data to the tree view."""
        # Settings
        settings = QtGui.QTreeWidgetItem(self._tree)
        settings.setText(0, "Settings")
        # Add some dummy data
        for i in range(0, 15):
            item = QtGui.QTreeWidgetItem(settings)
            item.setText(0, "Item " + str(i))
            item.setText(1, str(random.randint(1, 100)))
            item.setText(2, "Deg/s^2")

        # Data objects
        data = QtGui.QTreeWidgetItem(self._tree)
        data.setText(0, "Data Objects")
        # Add some dummy data
        for i in range(1, 35):
            item = QtGui.QTreeWidgetItem(data)
            item.setText(0, "Item " + str(i))
            item.setText(1, str(random.randint(1, 100)))
            item.setText(2, "Deg/s^2")          
