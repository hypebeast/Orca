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

from PyQt4 import QtGui

#try:
#    from PyQt4 import QtGui
#except ImportError:
#    print "No PyQt found!"
#    import sys
#    sys.exit(2)


class FlightControlPage(QtGui.QWidget):
    def __init__(self):
        super(FlightControlPage, self).__init__()

        self.createUI()

    def createUI(self):
        pass

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
        qp.setBrush(QtGui.QColor(128, 128, 128))
        qp.drawRect(0, 0, width, height)
