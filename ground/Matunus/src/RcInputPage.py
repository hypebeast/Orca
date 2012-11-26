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

__author__ = 'Sebastian Ruml'


try:
    from PyQt4 import QtGui, QtCore
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)


class RcInputPage(QtGui.QWidget):
    def __init__(self, parent=None):
        super(RcInputPage, self).__init__(parent)

        self.createUI()

    def createUI(self):
    	mainLayout = QtGui.QVBoxLayout()

    	layout = QtGui.QHBoxLayout()
    	label = QtGui.QLabel("<b>Function</b>")
    	layout.addWidget(label)
    	layout.addSpacing(45)
    	label = QtGui.QLabel("<b>Min</b<")
    	layout.addWidget(label)
    	layout.addSpacing(200)
    	label = QtGui.QLabel("<b>Neutral</b>")
    	layout.addWidget(label)
    	layout.addSpacing(185)
    	label = QtGui.QLabel("<b>Max</b>")
    	layout.addWidget(label)
    	layout.addStretch(1)
    	mainLayout.addLayout(layout)
    	mainLayout.addSpacing(10)

    	layout = QtGui.QHBoxLayout()
    	label = QtGui.QLabel("Throttle")
    	layout.addWidget(label)
    	layout.addSpacing(50)
    	self.leThrottleMin = QtGui.QLineEdit()
    	self.leThrottleMin.setMaximumWidth(70)
    	layout.addWidget(self.leThrottleMin)
    	layout.addSpacing(15)
    	self.throttleNeutral = QtGui.QSlider(QtCore.Qt.Horizontal)
    	self.throttleNeutral.setMinimumWidth(350)
    	layout.addWidget(self.throttleNeutral)
    	layout.addSpacing(15)
    	self.leThrottleMax = QtGui.QLineEdit()
    	self.leThrottleMax.setMaximumWidth(70)
    	layout.addWidget(self.leThrottleMax)
    	layout.addSpacing(5)
    	self.throttleNeutralValue = QtGui.QLabel("0")
    	layout.addWidget(self.throttleNeutralValue)

    	layout.addStretch(1)
    	mainLayout.addLayout(layout)

    	layout = QtGui.QHBoxLayout()
    	label = QtGui.QLabel("Roll")
    	layout.addWidget(label)
    	layout.addSpacing(70)
    	self.rollMin = QtGui.QLineEdit()
    	self.rollMin.setMaximumWidth(70)
    	layout.addWidget(self.rollMin)
    	layout.addSpacing(15)
    	self.rollNeutral = QtGui.QSlider(QtCore.Qt.Horizontal)
    	self.rollNeutral.setMinimumWidth(350)
    	layout.addWidget(self.rollNeutral)
    	layout.addSpacing(15)
    	self.rollMax = QtGui.QLineEdit()
    	self.rollMax.setMaximumWidth(70)
    	layout.addWidget(self.rollMax)
    	layout.addSpacing(5)
    	self.rollNeutralValue = QtGui.QLabel("0")
    	layout.addWidget(self.rollNeutralValue)

    	layout.addStretch(1)
    	mainLayout.addLayout(layout)

    	layout = QtGui.QHBoxLayout()
    	label = QtGui.QLabel("Pitch")
    	layout.addWidget(label)
    	layout.addSpacing(64)
    	self.pitchMin = QtGui.QLineEdit()
    	self.pitchMin.setMaximumWidth(70)
    	layout.addWidget(self.pitchMin)
    	layout.addSpacing(15)
    	self.pitchNeutral = QtGui.QSlider(QtCore.Qt.Horizontal)
    	self.pitchNeutral.setMinimumWidth(350)
    	layout.addWidget(self.pitchNeutral)
    	layout.addSpacing(15)
    	self.pitchMax = QtGui.QLineEdit()
    	self.pitchMax.setMaximumWidth(69)
    	layout.addWidget(self.pitchMax)
    	layout.addSpacing(5)
    	self.pitchNeutralValue = QtGui.QLabel("0")
    	layout.addWidget(self.pitchNeutralValue)

    	layout.addStretch(1)
    	mainLayout.addLayout(layout)

    	layout = QtGui.QHBoxLayout()
    	label = QtGui.QLabel("Yaw")
    	layout.addWidget(label)
    	layout.addSpacing(68)
    	self.yawMin = QtGui.QLineEdit()
    	self.yawMin.setMaximumWidth(70)
    	layout.addWidget(self.yawMin)
    	layout.addSpacing(15)
    	self.yawNeutral = QtGui.QSlider(QtCore.Qt.Horizontal)
    	self.yawNeutral.setMinimumWidth(350)
    	layout.addWidget(self.yawNeutral)
    	layout.addSpacing(15)
    	self.yawMax = QtGui.QLineEdit()
    	self.yawMax.setMaximumWidth(70)
    	layout.addWidget(self.yawMax)
    	layout.addSpacing(5)
    	self.yawNeutralValue = QtGui.QLabel("0")
    	layout.addWidget(self.yawNeutralValue)

    	layout.addStretch(1)
    	mainLayout.addLayout(layout)
    	
    	mainLayout.addStretch(1)
    	self.setLayout(mainLayout)