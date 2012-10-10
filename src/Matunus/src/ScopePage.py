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


import defs
from ScopeWidget import ScopeWidget
from logger import Logger


class ScopePage(QtGui.QWidget):
    """
    This page holds all scope widgetes.
    """
    def __init__(self, boardController):
        super(ScopePage, self).__init__()

        if boardController is None:
            raise "Error!"

        self.boardController = boardController
        self.scopes = list()
        self.app_defs = defs.AppDefs()
        self._logger = Logger()
        self._createUi()

    def _createUi(self):
        """Initializes the UI."""
        self.mainLayout = QtGui.QVBoxLayout()
        self._addScopes()

        groupBox = QtGui.QGroupBox("Scope Control")
        groupBox.setMaximumHeight(90)
        groupBox.setMinimumHeight(90)
        layout = QtGui.QHBoxLayout()
        groupBox.setLayout(layout)
        self.startButton = QtGui.QPushButton("Start")
        layout.addWidget(self.startButton)
        self.stopButton = QtGui.QPushButton("Stop")
        layout.addWidget(self.stopButton)
        layout.addStretch()
        self.mainLayout.addWidget(groupBox)

        self.setLayout(self.mainLayout)

    def _addScopes(self):
        """Adds all scopes to the GUI."""
        self.scope1 = ScopeWidget(['test1', 'test2'], self.boardController)
        self.mainLayout.addWidget(self.scope1)
        self.scopes.append(self.scope1)

        self.scope2 = ScopeWidget(['test3', 'test4'], self.boardController)
        self.mainLayout.addWidget(self.scope2)
        self.scopes.append(self.scope2)

    def start(self):
        """Starts scoping."""
        pass

    def stop(self):
        """Stops scoping."""
        pass