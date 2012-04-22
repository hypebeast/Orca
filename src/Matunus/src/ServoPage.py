__author__ = 'Sebastian Ruml'

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


# This is only needed for Python v2 but is harmless for Python v3.
import sip
sip.setapi('QVariant', 2)

try:
    from PyQt4 import QtGui
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)

from ServoControlWidget import ServoControlWidget


class ServoPage(QtGui.QWidget):
    def __init__(self, serial=None):
        super(ServoPage, self).__init__()

        self.serial_connection = serial
        self.createUi()

    def createUi(self):
        mainLayout = QtGui.QVBoxLayout()

        palette = QtGui.QPalette()
        palette.setColor(QtGui.QPalette.Foreground, QtGui.QColor(111, 88, 100))

        # Servo 1
        groupBox1 = QtGui.QGroupBox("Servo 1")
        groupBox1.setMinimumHeight(70)
        self.servo1Control = ServoControlWidget(self.serial_connection)
        hBox = QtGui.QHBoxLayout()
        hBox.setMargin(2)
        hBox.addWidget(self.servo1Control)
        groupBox1.setLayout(hBox)
        mainLayout.addWidget(groupBox1)

        # Servo 2
        groupBox2 = QtGui.QGroupBox("Servo 2")
        groupBox2.setMinimumHeight(70)
        self.servo2Control = ServoControlWidget(self.serial_connection)
        hBox = QtGui.QHBoxLayout()
        hBox.setMargin(2)
        hBox.addWidget(self.servo2Control)
        groupBox2.setLayout(hBox)
        mainLayout.addWidget(groupBox2)
        mainLayout.addStretch()

        self.setLayout(mainLayout)

    def startMoveServo1(self):
        print "Start moving servo 1"
        self.moveServo(1, 0)

    def moveServo(self, servo_nr, position):
        pass

    def dialServo1ValueChanged(self, value):
        self.setPositionServo1.display(value)
