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


# This is only needed for Python v2 but is harmless for Python v3.
import sip
sip.setapi('QVariant', 2)

try:
    from PyQt4 import QtGui
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)

from PositionBar import PositionBar


class ServoControlWidget(QtGui.QWidget):
    def __init__(self, controller=None, servo_nr=1):
        super(ServoControlWidget, self).__init__()

        if controller is None:
            raise Exception

        self.controller = controller

        self.servo_nr = servo_nr
        self.minValue = 0
        self.maxValue = 180

        self.createUi()

    def createUi(self):
        palette = QtGui.QPalette()
        palette.setColor(QtGui.QPalette.Foreground, QtGui.QColor(111, 88, 100))

        layout = QtGui.QHBoxLayout()
        layout.setMargin(2)
        layout.addSpacing(5)
        
        lActualPosition = QtGui.QLabel("<b>Current Value</b>")
        layout.addWidget(lActualPosition)
        layout.addSpacing(5)

        self.actualPositionServo = QtGui.QLCDNumber()
        self.actualPositionServo.setMaximumHeight(25)
        self.actualPositionServo.setPalette(palette)
        self.actualPositionServo.setSegmentStyle(QtGui.QLCDNumber.Filled)
        layout.addWidget(self.actualPositionServo)
        layout.addSpacing(30)

        lMove1 = QtGui.QLabel("<b>Setpoint</b>")
        layout.addWidget(lMove1)
        layout.addSpacing(5)
        
        self.positionBar = PositionBar()
        self.positionBar.setMinimumWidth(200)
        self.positionBar.setMinimumHeight(15)
        self.positionBar.setMaximumHeight(30)
        self.positionBar.setValue(0)
        self.positionBar.setBarColor(QtGui.QColor(0,0,255))
        self.positionBar.valueChanged.connect(self.positionBarValueChanged)
        layout.addWidget(self.positionBar)
        #layout.addSpacing(50)
        
        self.leServoPosition = QtGui.QLineEdit()
        self.leServoPosition.setMaximumWidth(50)
        layout.addWidget(self.leServoPosition)
        layout.addSpacing(30)
        
        self.startServo = QtGui.QPushButton("Move")
        self.startServo.clicked.connect(self.startServoClicked)
        layout.addWidget(self.startServo)
        layout.addStretch()
        self.setLayout(layout)

    def runMoveServo(self, position):
        if not self.checkValue(float(position)):
            return

        #command = SetServoPosCommand(self.servo_nr, position)
        #self.controller.writeCommand(command)
        self.controller.setServoPos(self.servo_nr, position)

    def setMinValue(self, value):
        self.minValue = value

    def setMaxValue(self, value):
        self.maxValue = value
    
    def checkValue(self, value):
        if value < self.minValue or value > self.maxValue:
            return False
        else:
            return True

    def startServoClicked(self):
        try:
            position = self.leServoPosition.displayText()
        except:
            return

        if not self.checkValue(float(position)):
            return

        self.positionBar.setValue(int(position))
        #self.actualPositionServo.display(position)

        self.runMoveServo(int(position))

    def positionBarValueChanged(self, value):
        self.leServoPosition.setText(str(value))

        self.runMoveServo(value)
