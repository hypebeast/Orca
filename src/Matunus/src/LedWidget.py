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

__author__ = "Sebastian Ruml"

try:
    from PyQt4 import QtGui, QtCore, QtSvg
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)

import os

import defs


GREEN = 0
RED = 1

class LedWidget(QtGui.QWidget):
    def __init__(self):
        super(LedWidget, self).__init__()

        self.createUI()

    def createUI(self):
        self.setMinimumSize(20, 20)

        app_defs = defs.AppDefs()
        self.colors = [os.path.join(app_defs.ArtworkPath, "led_green.svg"),
                        os.path.join(app_defs.ArtworkPath, "led_red.svg")]
        self.color = self.colors[0]

    def paintEvent(self, e):
        qp = QtGui.QPainter()
        qp.begin(self)
        self.drawWidget(qp)
        qp.end()

    def drawWidget(self, qp):
        renderer = QtSvg.QSvgRenderer()
        renderer.load(self.color)
        renderer.render()

    def setColor(self, color):
        if color < len(self.colors) or color > len(self.colors):
            return
        self.color = self.colors[color]
