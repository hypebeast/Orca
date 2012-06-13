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

# This is only needed for Python v2 but is harmless for Python v3.
import sip
sip.setapi('QVariant', 2)

try:
    from PyQt4 import QtGui, QtSvg, QtCore
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)

import defs

class FlightDisplay(QtGui.QWidget):
    def __init__(self):
        super(FlightDisplay, self).__init__()

        self.appDefs = defs.AppDefs()

        self.createUi()

    def createUi(self):
        self.mainLayout = QtGui.QVBoxLayout()
        self.mainLayout.setMargin(0)

        backgroundFile = os.path.join(self.appDefs.ArtworkPath, "grey_background.svg")
        background = QtSvg.QSvgWidget(backgroundFile)
        background.setSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding)
        print "Background: " + str(background.size().height())
        background.setGeometry(0, 0, self.width(), self.height())
        background.updateGeometry()
        background.resize(self.width(), self.height())
        self.mainLayout.addWidget(background)

        #textEdit = QtGui.QTextEdit()
        #self.mainLayout.addWidget(textEdit)

        self.setLayout(self.mainLayout)

    def paintEvent(self, e):
        pass

    def sizeHint(self):
        return QtCore.QSize(1000, 600)
