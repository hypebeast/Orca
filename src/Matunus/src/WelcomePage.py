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
    from PyQt4 import QtGui, QtSvg
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)

import defs

class MainPage(QtGui.QWidget):
    def __init__(self):
        super(MainPage, self).__init__()

        self._createUi()

    def _createUi(self):
        mainLayout = QtGui.QVBoxLayout()

        label = QtGui.QLabel("<b><font size=\"8\" color='black'>Matunus</font></b>")
        mainLayout.addWidget(label)
        label = QtGui.QLabel("<font size=\"5\" color='black'>Orca Ground Control Station</font>")
        mainLayout.addWidget(label)

        self.app_defs = defs.AppDefs()
        self.background = os.path.join(self.app_defs.ArtworkPath, "welcome_background.svg")

        # Status group box
        #groupBoxStatus = QtGui.QGroupBox("Status")
        #vLayout = QtGui.QVBoxLayout()
        #systemStatusLayout = QtGui.QHBoxLayout()
        #lSystemStatus = QtGui.QLabel("System.Status:")
        #systemStatusLayout.addWidget(lSystemStatus)
        #systemStatusLayout.insertSpacing(1, 10)
        #self.systemStatus = StatusLabel()
        #self.systemStatus.setText("Disconnected")
        #self.systemStatus.setMaximumHeight(40)
        #self.systemStatus.setMinimumWidth(100)
        #systemStatusLayout.addWidget(self.systemStatus)
        #systemStatusLayout.insertSpacing(3, 30)
        #lSystemMessage = QtGui.QLabel("System.Message:")
        #systemStatusLayout.addWidget(lSystemMessage)
        #systemStatusLayout.insertSpacing(5, 20)
        #self.systemMessage = StatusLabel()
        #self.systemMessage.setText("None")
        #self.systemMessage.setMaximumHeight(40)
        #self.systemMessage.setMinimumWidth(500)
        #systemStatusLayout.addWidget(self.systemMessage)
        #systemStatusLayout.addStretch()
        #vLayout.addItem(systemStatusLayout)

        #vLayout.addStretch()
        #groupBoxStatus.setLayout(vLayout)

        #mainLayout.addWidget(groupBoxStatus)
        mainLayout.addStretch()
        self.setLayout(mainLayout)

    def paintEvent(self, e):
        qp = QtGui.QPainter()
        qp.begin(self)
        #self._drawWidget(qp)
        qp.end()

    def _drawWidget(self, qp):
        svgRenderer = QtSvg.QSvgRenderer()
        svgRenderer.load(self.background)
        svgRenderer.render(qp)

    def setSystemStatus(self, text):
        pass
        #self.systemStatus.setText(text)

    def setSystemMessage(self, message):
        pass
        #self.systemMessage.setText(message)
