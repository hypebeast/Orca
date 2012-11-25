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

try:
    from PyQt4 import QtGui, QtCore
    from PyQt4.Qt import Qt
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)


class PositionBar(QtGui.QWidget):
    # Signals
    valueChanged = QtCore.pyqtSignal(int)

    def __init__(self):
        super(PositionBar, self).__init__()

        self.value = 0
        self.maxValue = 180
        self.minValue = 0
        self._moveable = True

        self.HORIZONTAL = 0
        self.VERTICAL = 1

        self.orientation = self.HORIZONTAL
        self.cursorWidth = 7
        self.cursorHeight = 7
        self.boarderSize = 8
        self.frameThickness = 1

        self.cursorCurrentPosX = 0
        self.cursor1CurrentPosY = 0
        self.cursor2CurrentPosY = 0

        self.isCursorClicked = False
        self.isMousePressed = False
        self.mouseClickedPoint = None
        
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

    def getBarWidth(self):
        return self.size().width() - 2*self.boarderSize

    def setMoveable(self, value):
        self._moveable = value

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

        qp.setPen(QtCore.Qt.NoPen)
        qp.setBrush(self.barColor)
        qp.drawRect(self.boarderSize+self.frameThickness, self.boarderSize+self.frameThickness,
            activeBarWidth, barHeight-self.frameThickness)

        # Draw the top cursor
        qp.setPen(self.cursorColor)
        qp.setBrush(self.cursorColor)
        self.cursor1CurrentPosY = self.boarderSize - self.cursorHeight
        self.cursorCurrentPosX = activeBarWidth + self.boarderSize
        p1 = QtCore.QPoint(self.cursorCurrentPosX-self.cursorWidth/2, self.cursor1CurrentPosY)
        p2 = QtCore.QPoint(self.cursorCurrentPosX+self.cursorWidth/2, self.cursor1CurrentPosY)
        p3 = QtCore.QPoint(self.cursorCurrentPosX, self.cursor1CurrentPosY + self.cursorHeight)
        qp.drawPolygon(p1, p2, p3)

        # Draw the bottom cursor
        self.cursor2CurrentPosY = height - (self.boarderSize - self.cursorHeight)
        p4 = QtCore.QPoint(self.cursorCurrentPosX-self.cursorWidth/2, self.cursor2CurrentPosY)
        p5 = QtCore.QPoint(self.cursorCurrentPosX+self.cursorWidth/2, self.cursor2CurrentPosY)
        p6 = QtCore.QPoint(self.cursorCurrentPosX, self.cursor2CurrentPosY - self.cursorHeight)
        qp.drawPolygon(p4, p5, p6)

        # Draw the connection line
        pen = QtGui.QPen()
        pen.setStyle(QtCore.Qt.SolidLine)
        pen.setBrush(self.cursorColor)
        pen.setWidth(1)
        qp.setPen(pen)
        qp.drawLine(p3, p6)

    def mouseMoveEvent(self, event):
        if self.isCursorClicked and self._moveable:
            cursorPosX = event.pos().x()

            # Check bounds
            if cursorPosX < 0:
                cursorPosX = 0
            elif cursorPosX > self.getBarWidth():
                cursorPosX = self.getBarWidth()

            # Calculate the new value
            valueRange = self.maxValue - self.minValue
            stepPerPercentage = float(self.getBarWidth() / 100.0)
            newPosPercentage = (float(cursorPosX) / float(self.getBarWidth())) * 100.0
            self.value = float((valueRange / 100.0) * newPosPercentage)

            # Repaint the widget
            self.repaint()

            # Emit signal
            self.valueChanged.emit(self.value)

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.isMousePressed = True
            self.mouseClickedPoint = event.pos()

            # Create bounding box for mouse click (upper and lower play head)
            upperLeft = QtCore.QPoint(self.cursorCurrentPosX - 5, self.cursor1CurrentPosY)
            upperRight = QtCore.QPoint(self.cursorCurrentPosX + 5, self.cursor1CurrentPosY)
            lowerRight = QtCore.QPoint(self.cursorCurrentPosX + 5, self.cursor2CurrentPosY)
            lowerLeft = QtCore.QPoint(self.cursorCurrentPosX - 5, self.cursor2CurrentPosY)

            # Check if box contains mouse click for cursor
            if self.mouseClickedPoint.x() > upperLeft.x() and self.mouseClickedPoint.x() < upperRight.x():
                if self.mouseClickedPoint.y() > upperLeft.y() and self.mouseClickedPoint.y() < lowerRight.y():
                    self.isCursorClicked = True

    def mouseReleaseEvent(self, event):
        if event.button() and self.isMousePressed:
            self.isMousePressed = False
            self.isCursorClicked = False

