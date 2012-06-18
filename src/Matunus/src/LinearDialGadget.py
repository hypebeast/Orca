# Copyright (C) 2012 Sebastian Ruml <sebastian.ruml@gmail.com>
#
# This file is part of the Matunus project (part of the Orcacopter project)
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

__author__ = 'Sebastian Ruml'

import os

# This is only needed for Python v2 but is harmless for Python v3.
import sip
sip.setapi('QVariant', 2)

try:
    from PyQt4 import QtGui, QtCore, QtSvg
    from PyQt4.Qt import Qt
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)


import defs


class LinearDialGadget(QtGui.QWidget):
    def __init__(self):
        super(LinearDialGadget, self).__init__()

        self.initUI()

    def initUI(self):
    	self.app_defs = defs.AppDefs()
    	self.sourceSvgFile = os.path.join(self.app_defs.ArtworkPath, "lineardial-vertical.svg")

    	self.setMinimumWidth(50)
    	self.setMinimumHeight(300)

    def paintEvent(self, e):
    	qp = QtGui.QPainter()
        qp.begin(self)
        self.drawWidget(qp)
        qp.end()

    def drawWidget(self, qp):
    	svgRenderer = QtSvg.QSvgRenderer()
    	svgRenderer.load(self.sourceSvgFile)
    	svgRenderer.render(qp)