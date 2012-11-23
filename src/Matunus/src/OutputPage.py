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


try:
    from PyQt4 import QtGui
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)

from ServoControlWidget import ServoControlWidget


class OutputPage(QtGui.QWidget):
    def __init__(self, controller=None):
        super(OutputPage, self).__init__()

        if controller is None:
            raise Exception
            
        self.controller = controller
        self.controller.board_status_updated.connect(self._onStatusUpdated)
        self.createUi()

    def createUi(self):
        mainLayout = QtGui.QVBoxLayout()

        palette = QtGui.QPalette()
        palette.setColor(QtGui.QPalette.Foreground, QtGui.QColor(111, 88, 100))

        # Channel 1
        groupBox1 = QtGui.QGroupBox("Channel 1")
        groupBox1.setMinimumHeight(70)
        self.channel1Control = ServoControlWidget(self.controller, servo_nr=1)
        hBox = QtGui.QHBoxLayout()
        hBox.setMargin(2)
        hBox.addWidget(self.channel1Control)
        groupBox1.setLayout(hBox)
        mainLayout.addWidget(groupBox1)

        # Channel 2
        groupBox2 = QtGui.QGroupBox("Channel 2")
        groupBox2.setMinimumHeight(70)
        self.channel2Control = ServoControlWidget(self.controller, servo_nr=2)
        hBox = QtGui.QHBoxLayout()
        hBox.setMargin(2)
        hBox.addWidget(self.channel2Control)
        groupBox2.setLayout(hBox)
        mainLayout.addWidget(groupBox2)

        # Channel 3
        groupBox = QtGui.QGroupBox("Channel 3")
        groupBox.setMinimumHeight(70)
        self.channel3Control = ServoControlWidget(self.controller, servo_nr=3)
        hBox = QtGui.QHBoxLayout()
        hBox.setMargin(2)
        hBox.addWidget(self.channel3Control)
        groupBox.setLayout(hBox)
        mainLayout.addWidget(groupBox)

        # Channel 4
        groupBox = QtGui.QGroupBox("Channel 4")
        groupBox.setMinimumHeight(70)
        self.channel4Control = ServoControlWidget(self.controller, servo_nr=4)
        hBox = QtGui.QHBoxLayout()
        hBox.setMargin(2)
        hBox.addWidget(self.channel4Control)
        groupBox.setLayout(hBox)
        mainLayout.addWidget(groupBox)

        # Channel 5
        groupBox = QtGui.QGroupBox("Channel 5")
        groupBox.setMinimumHeight(70)
        self.channel5Control = ServoControlWidget(self.controller, servo_nr=5)
        hBox = QtGui.QHBoxLayout()
        hBox.setMargin(2)
        hBox.addWidget(self.channel5Control)
        groupBox.setLayout(hBox)
        mainLayout.addWidget(groupBox)

        # Channel 6
        groupBox = QtGui.QGroupBox("Channel 6")
        groupBox.setMinimumHeight(70)
        self.channel6Control = ServoControlWidget(self.controller, servo_nr=6)
        hBox = QtGui.QHBoxLayout()
        hBox.setMargin(2)
        hBox.addWidget(self.channel6Control)
        groupBox.setLayout(hBox)
        mainLayout.addWidget(groupBox)

        # Configuration
        #groupBox3 = QtGui.QGroupBox("Configuration")
        #groupBox3.setMinimumHeight(200)
        #mainLayout.addWidget(groupBox3)
        
        mainLayout.addStretch()
        self.setLayout(mainLayout)

    def startMoveServo1(self):
        print "Start moving servo 1"
        self.moveServo(1, 0)

    def moveServo(self, servo_nr, position):
        pass

    def dialServo1ValueChanged(self, value):
        self.setPositionServo1.display(value)

    def update(self, data):
        pass

    def _onStatusUpdated(self):
        self.channel1Control.updateServoPosition(self.controller.boardStatus.outputChannel1)
        self.channel2Control.updateServoPosition(self.controller.boardStatus.outputChannel2)
        self.channel3Control.updateServoPosition(self.controller.boardStatus.outputChannel3)
        self.channel4Control.updateServoPosition(self.controller.boardStatus.outputChannel4)
        self.channel5Control.updateServoPosition(self.controller.boardStatus.outputChannel5)
        self.channel6Control.updateServoPosition(self.controller.boardStatus.outputChannel6)
