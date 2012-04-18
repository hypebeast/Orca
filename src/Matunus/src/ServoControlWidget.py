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


# This is only needed for Python v2 but is harmless for Python v3.
import sip
sip.setapi('QVariant', 2)

try:
    from PyQt4 import QtGui
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)

from serial_api import CommandMessage, CommandType


class ServoControlWidget(QtGui.QWidget):
    def __init__(self, serial=None):
        super(ServoControlWidget, self).__init__()

        self.serial_connection = serial

        self.minValue = 0
        self.maxValue = 180

        self.createUi()

    def createUi(self):
        palette = QtGui.QPalette()
        palette.setColor(QtGui.QPalette.Foreground, QtGui.QColor(111, 88, 100))

        layout = QtGui.QHBoxLayout()
        layout.setMargin(2)
        layout.addSpacing(5)
        lActualPosition = QtGui.QLabel("Actual Pos:")
        layout.addWidget(lActualPosition)
        self.actualPositionServo = QtGui.QLCDNumber()
        self.actualPositionServo.setMaximumHeight(25)
        self.actualPositionServo.setPalette(palette)
        self.actualPositionServo.setSegmentStyle(QtGui.QLCDNumber.Filled)
        layout.addWidget(self.actualPositionServo)
        layout.insertSpacing(4, 40)
        self.positionBar = QtGui.QProgressBar()
        self.positionBar.setMinimum(self.minValue)
        self.positionBar.setMaximum(self.maxValue)
        layout.addWidget(self.positionBar)
        lMove1 = QtGui.QLabel("Move")
        layout.addWidget(lMove1)
        self.leServoPosition = QtGui.QLineEdit()
        self.leServoPosition.setMaximumWidth(50)
        layout.addWidget(self.leServoPosition)
        self.startServo = QtGui.QPushButton("Start")
        self.startServo.clicked.connect(self.startMoveServo)
        layout.addWidget(self.startServo)
        layout.addStretch()
        self.setLayout(layout)

    def startMoveServo(self):
        try:
            position = self.leServoPosition.displayText()
        except:
            return

        if not self.checkValue(float(position)):
            return

        print "Position: " + position
        command = CommandMessage(CommandType.SETSERVOPOS)
        command.addArgument(str(1))
        command.addArgument(str(position))
        self.serial_connection.writeCommand(command)


    def checkValue(self, value):
        if value < self.minValue or value > self.maxValue:
            return False
        else:
            return True
