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


try:
    from PyQt4 import QtGui, QtSvg, QtCore
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)

import defs

from gadgets.airspeed.airspeed_gadget import AirspeedGadget


class FlightDisplay(QtGui.QWidget):
    def __init__(self):
        super(FlightDisplay, self).__init__()

        self.appDefs = defs.AppDefs()

        self._createUi()

    def _createUi(self):
        #self.view = QtGui.QGraphicsView()
        #self.scene = QtGui.QGraphicsScene()

        #self.view.setRenderHint(QtGui.QPainter.Antialiasing)
        #self.view.setTransformationAnchor(QtGui.QGraphicsView.AnchorUnderMouse)
        #self.view.setResizeAnchor(QtGui.QGraphicsView.AnchorViewCenter)
        #self.view.setScene(self.scene)

        #brush = QtGui.QBrush(QtCore.Qt.darkGray)
        #self.view.setBackgroundBrush(brush)
        
        #self.view.setParent(self, QtCore.Qt.FramelessWindowHint)

        mainLayout = QtGui.QVBoxLayout()

        layout = QtGui.QHBoxLayout()
        #self.vertBar = LinearDialGadget()
        self.airspeedGadget = AirspeedGadget()
        layout.addWidget(self.airspeedGadget)
        layout.addStretch()
        mainLayout.addLayout(layout)

        mainLayout.addStretch()
        self.setLayout(mainLayout)

    def paintEvent(self, e):
        qp = QtGui.QPainter()
        qp.begin(self)
        self.drawBackground(qp)
        qp.end()

    def drawBackground(self, qp):
        size = self.size()
        width = size.width()
        height = size.height()

        # Draw the background
        qp.setBrush(QtGui.QColor("#646464"))
        qp.drawRect(0, 0, width, height)

    def resizeEvent(self, event):
        pass
        #width = self.size().width()
        #height = self.size().height()

        #self.view.setSceneRect(0,0, width, height)
        #self.view.resize(width, height)
