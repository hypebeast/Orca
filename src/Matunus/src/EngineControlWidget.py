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

from serial_api import CommandMessage, CommandType
from PositionBar import PositionBar


class Engines:
    EngineLeft = 0
    EngineRight = 1
    EngineTail = 2


class EngineControlWidget(QtGui.QWidget):
    def __init__(self, serial=None, engine_nr=Engines.EngineLeft):
        super(EngineControlWidget, self).__init__()

        self.serial_connection = serial

        self.engine_nr = engine_nr
        self.minValue = 0
        self.maxValue = 100

        self.createUi()

    def createUi(self):
        palette = QtGui.QPalette()
        palette.setColor(QtGui.QPalette.Foreground, QtGui.QColor(111, 88, 100))

        layout = QtGui.QHBoxLayout()
        layout.setMargin(2)
        layout.addSpacing(5)
        
        lCurrentVelocity = QtGui.QLabel("<b>Current Velocity</b>")
        layout.addWidget(lCurrentVelocity)
        layout.addSpacing(5)
        
        self.currentVelocity = QtGui.QLCDNumber()
        self.currentVelocity.setMaximumHeight(25)
        self.currentVelocity.setPalette(palette)
        self.currentVelocity.setSegmentStyle(QtGui.QLCDNumber.Filled)
        layout.addWidget(self.currentVelocity)
        layout.addSpacing(30)
        
        lSetpoint = QtGui.QLabel("<b>Setpoint</b>")
        layout.addWidget(lSetpoint)
        layout.addSpacing(5)

        self.positionBar = PositionBar()
        self.positionBar.setMinimumWidth(200)
        self.positionBar.setMinimumHeight(15)
        self.positionBar.setMaximumHeight(30)
        self.positionBar.setValue(0)
        self.positionBar.setBarColor(QtGui.QColor(0,0,255))
        self.positionBar.valueChanged.connect(self.positionBarValueChanged)
        layout.addWidget(self.positionBar)
        #layout.addSpacing(5)

        self.leServoPosition = QtGui.QLineEdit()
        self.leServoPosition.setMaximumWidth(50)
        layout.addWidget(self.leServoPosition)
        layout.addSpacing(30)

        self.bStartEngine = QtGui.QPushButton("Start")
        self.bStartEngine.clicked.connect(self.startEngine)
        layout.addWidget(self.bStartEngine)
        
        layout.addStretch()
        self.setLayout(layout)

    def startEngine(self):
        try:
            position = self.leServoPosition.displayText()
        except:
            return

        if not self.checkValue(float(position)):
            return

        self.positionBar.setValue(int(position))
        self.actualPositionServo.display(position)
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

    def positionBarValueChanged(self, value):
        self.leServoPosition.setText(str(value))
