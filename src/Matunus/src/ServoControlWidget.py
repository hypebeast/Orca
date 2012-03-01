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

class ServoControlWidget(QtGui.QWidget):
    def __init__(self):
        super(ServoControlWidget, self).__init__()

        self.minValue = 0
        self.maxValue = 250

        self.createUi()

    def createUi(self):
        palette = QtGui.QPalette()
        palette.setColor(QtGui.QPalette.Foreground, QtGui.QColor(111, 88, 100))

        layout = QtGui.QHBoxLayout()
        layout.setMargin(1)
        lActualPosition = QtGui.QLabel("Actual Pos:")
        layout.addWidget(lActualPosition)
        self.actualPositionServo = QtGui.QLCDNumber()
        self.actualPositionServo.setMaximumHeight(25)
        self.actualPositionServo.setPalette(palette)
        self.actualPositionServo.setSegmentStyle(QtGui.QLCDNumber.Filled)
        layout.addWidget(self.actualPositionServo)
        lSetPosition = QtGui.QLabel("Set Pos:")
        layout.addWidget(lSetPosition)
        self.setPositionServo = QtGui.QLCDNumber()
        self.setPositionServo.setMaximumHeight(25)
        self.setPositionServo.setPalette(palette)
        self.setPositionServo.setSegmentStyle(QtGui.QLCDNumber.Filled)
        layout.addWidget(self.setPositionServo)
        layout.insertSpacing(4, 40)
        lMove1 = QtGui.QLabel("Move")
        layout.addWidget(lMove1)
        self.dialServo = QtGui.QDial()
        self.dialServo.setMinimum(0)
        self.dialServo.setMaximum(250)
        self.dialServo.setSingleStep(1)
        self.dialServo.setWrapping(False)
        self.dialServo.valueChanged.connect(self.dialServoValueChanged)
        layout.addWidget(self.dialServo)
        self.leServoPosition = QtGui.QLineEdit()
        self.leServoPosition.setMaximumWidth(50)
        layout.addWidget(self.leServoPosition)
        self.startServo = QtGui.QPushButton("Start")
        self.startServo.clicked.connect(self.startMoveServo)
        layout.addWidget(self.startServo)
        layout.addStretch()
        self.setLayout(layout)

    def dialServoValueChanged(self, value):
        self.setPositionServo.display(value)

    def startMoveServo(self):
        try:
            position = int(self.leServoPosition.displayText())
        except:
            return

        if not self.checkValue(position):
            return

        self.setPositionServo.display(str(position))
        self.dialServo.setValue(position)
        print position

    def checkValue(self, value):
        if value < self.minValue or value > self.maxValue:
            return False
        else:
            return True
