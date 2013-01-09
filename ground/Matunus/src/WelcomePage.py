__author__ = 'Sebastian Ruml'

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

import os

try:
    from PyQt4 import QtGui
    from PyQt4 import QtDeclarative
    from PyQt4.QtCore import QUrl
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)

import defs

class WelcomePage(QtGui.QWidget):
    def __init__(self):
        super(WelcomePage, self).__init__()

        self.app_defs = defs.AppDefs()
        self._createUi()

    def _createUi(self):
        mainLayout = QtGui.QVBoxLayout()
        mainLayout.setMargin(0)

        # QML stuff
        self.mainViewFile = os.path.join(self.app_defs.QmlPath, "welcomepage", "welcomepage.qml")
        self.mainView = QtDeclarative.QDeclarativeView()
        self.mainView.setSource(QUrl(self.mainViewFile))
        self.mainView.setResizeMode(QtDeclarative.QDeclarativeView.SizeRootObjectToView)
        mainLayout.addWidget(self.mainView)

        self.setLayout(mainLayout)
