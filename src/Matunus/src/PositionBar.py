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

# This is only needed for Python v2 but is harmless for Python v3.
import sip
sip.setapi('QVariant', 2)

try:
    from PyQt4 import QtGui, QtCore
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)

class PositionBar(QtGui.QWidget):
    def __init__(self):
        super(PositionBar, self).__init__()

        self.value = 0
        self.maxValue = 180
        self.minValue = 0

        self.HORIZONTAL = 0
        self.VERTICAL = 1
        self.orientation = self.HORIZONTAL
        self.cursorWidth = 7
        self.cursorHeight = 7
        self.boarderSize = 8
        self.frameThickness = 1
        self.initUI()

    def initUI(self):
        self.setMinimumSize(1, 30)
        self.backgroundColor = QtGui.QColor(128, 128, 128)
        self.barColor = QtGui.QColor(255, 0, 0)
        self.frameColor = QtGui.QColor(0, 0, 0)
        self.cursorColor = QtGui.QColor(0, 0, 0)

    def setValue(self, value):
        self.value = value
        self.repaint()

    def setMaxValue(self, value):
        self.maxValue = value

    def setMinValue(self, value):
        self.minValue = value

    def setBackgroundColor(self, color):
        self.backgroundColor = color

    def setBarColor(self, color):
        self.barColor = color

    def setFrameColor(self, color):
        self.frameColor = color

    def setCursorColor(self, color):
        self.cursorColor = color

    def paintEvent(self, e):
        qp = QtGui.QPainter()
        qp.begin(self)
        self.drawWidget(qp)
        qp.end()

    def drawWidget(self, qp):
        size = self.size()
        width = size.width()
        height = size.height()

        if self.orientation == self.HORIZONTAL:
            pass
        else:
            pass

        barWidth = width - 2*self.boarderSize
        barHeight = height - 2*self.boarderSize

        # Draw the background
        qp.setPen(self.frameColor)
        qp.setBrush(self.backgroundColor)
        qp.drawRect(self.boarderSize, self.boarderSize, barWidth, barHeight)

        # Draw the bar
        valuePercentage = float(self.value / ((self.maxValue - self.minValue) / 100))
        activeBarWidth = float((barWidth / 100)) * valuePercentage
        print "Bar Width: " + str(barWidth)
        print activeBarWidth
        qp.setPen(QtCore.Qt.NoPen)
        qp.setBrush(self.barColor)
        qp.drawRect(self.boarderSize+self.frameThickness, self.boarderSize+self.frameThickness,
            activeBarWidth, barHeight-self.frameThickness)

        # Draw the top cursor
        qp.setPen(self.cursorColor)
        qp.setBrush(self.cursorColor)
        cursorYStart = self.boarderSize - self.cursorHeight
        cursorXStart = activeBarWidth + self.boarderSize
        p1 = QtCore.QPoint(cursorXStart-self.cursorWidth/2, cursorYStart)
        p2 = QtCore.QPoint(cursorXStart+self.cursorWidth/2, cursorYStart)
        p3 = QtCore.QPoint(cursorXStart, cursorYStart + self.cursorHeight)
        qp.drawPolygon(p1, p2, p3)

        # Draw the bottom cursor
        cursorYStart = height - (self.boarderSize - self.cursorHeight)
        p4 = QtCore.QPoint(cursorXStart-self.cursorWidth/2, cursorYStart)
        p5 = QtCore.QPoint(cursorXStart+self.cursorWidth/2, cursorYStart)
        p6 = QtCore.QPoint(cursorXStart, cursorYStart - self.cursorHeight)
        qp.drawPolygon(p4, p5, p6)

        # Draw the connection line
        pen = QtGui.QPen()
        pen.setStyle(QtCore.Qt.SolidLine)
        pen.setBrush(self.cursorColor)
        pen.setWidth(1)
        qp.setPen(pen)
        qp.drawLine(p3, p6)

    def mouseMoveEvent(self, event):
        print event.pos().x()
        print event.pos().y()

    def mousePressEvent(self, event):
        pass

    def mouseReleaseEvent(self, event):
        pass
