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

class ReceiverPage(QtGui.QWidget):
    def __init__(self):
        super(ReceiverPage, self).__init__()

        self.createUI()

    def createUI(self):
    	mainLayout = QtGui.QVBoxLayout()
    	mainLayout.setMargin(7)
    	mainLayout.addSpacing(5)

    	# Channel 1
    	lChannel1 = QtGui.QLabel("<b><font size=\"5\">Channel 1</font></b>")
    	mainLayout.addWidget(lChannel1)
    	layout = QtGui.QHBoxLayout()
    	layout.addSpacing(15)
    	lValue = QtGui.QLabel("Value")
    	layout.addWidget(lValue)
    	layout.addSpacing(15)
    	self.posBarChannel1 = PositionBar()
        self.posBarChannel1.setMinimumWidth(250)
        self.posBarChannel1.setMinimumHeight(30)
        self.posBarChannel1.setMaximumHeight(30)
        self.posBarChannel1.setValue(0)
        self.posBarChannel1.setMoveable(False)
        layout.addWidget(self.posBarChannel1)
    	self.lcdChannel1Value = QtGui.QLCDNumber()
        self.lcdChannel1Value.setMaximumHeight(25)
        self.lcdChannel1Value.setSegmentStyle(QtGui.QLCDNumber.Filled)
        layout.addWidget(self.lcdChannel1Value)
        layout.addStretch()
        mainLayout.addLayout(layout)

        mainLayout.addSpacing(15)

        # Channel 2
    	lChannel2 = QtGui.QLabel("<b><font size=\"5\">Channel 2</font></b>")
    	mainLayout.addWidget(lChannel2)
    	layout = QtGui.QHBoxLayout()
    	layout.addSpacing(15)
    	lValue = QtGui.QLabel("Value")
    	layout.addWidget(lValue)
    	layout.addSpacing(15)
    	self.posBarChannel2 = PositionBar()
        self.posBarChannel2.setMinimumWidth(250)
        self.posBarChannel2.setMinimumHeight(30)
        self.posBarChannel2.setMaximumHeight(30)
        self.posBarChannel2.setValue(0)
        self.posBarChannel2.setMoveable(False)
        layout.addWidget(self.posBarChannel2)
    	self.lcdChannel2Value = QtGui.QLCDNumber()
        self.lcdChannel2Value.setMaximumHeight(25)
        self.lcdChannel2Value.setSegmentStyle(QtGui.QLCDNumber.Filled)
        layout.addWidget(self.lcdChannel2Value)
        layout.addStretch()
        mainLayout.addLayout(layout)

        mainLayout.addSpacing(15)

        # Channel 3
    	lChannel3 = QtGui.QLabel("<b><font size=\"5\">Channel 3</font></b>")
    	mainLayout.addWidget(lChannel3)
    	layout = QtGui.QHBoxLayout()
    	layout.addSpacing(15)
    	lValue = QtGui.QLabel("Value")
    	layout.addWidget(lValue)
    	layout.addSpacing(15)
    	self.posBarChannel3 = PositionBar()
        self.posBarChannel3.setMinimumWidth(250)
        self.posBarChannel3.setMinimumHeight(30)
        self.posBarChannel3.setMaximumHeight(30)
        self.posBarChannel3.setValue(0)
        self.posBarChannel3.setMoveable(False)
        layout.addWidget(self.posBarChannel3)
    	self.lcdChannel3Value = QtGui.QLCDNumber()
        self.lcdChannel3Value.setMaximumHeight(25)
        self.lcdChannel3Value.setSegmentStyle(QtGui.QLCDNumber.Filled)
        layout.addWidget(self.lcdChannel3Value)
        layout.addStretch()
        mainLayout.addLayout(layout)

        mainLayout.addSpacing(15)

        # Channel 4
    	lChannel4 = QtGui.QLabel("<b><font size=\"5\">Channel 4</font></b>")
    	mainLayout.addWidget(lChannel4)
    	layout = QtGui.QHBoxLayout()
    	layout.addSpacing(15)
    	lValue = QtGui.QLabel("Value")
    	layout.addWidget(lValue)
    	layout.addSpacing(15)
    	self.posBarChannel4 = PositionBar()
        self.posBarChannel4.setMinimumWidth(250)
        self.posBarChannel4.setMinimumHeight(30)
        self.posBarChannel4.setMaximumHeight(30)
        self.posBarChannel4.setValue(0)
        self.posBarChannel4.setMoveable(False)
        layout.addWidget(self.posBarChannel4)
    	self.lcdChannel4Value = QtGui.QLCDNumber()
        self.lcdChannel4Value.setMaximumHeight(25)
        self.lcdChannel4Value.setSegmentStyle(QtGui.QLCDNumber.Filled)
        layout.addWidget(self.lcdChannel4Value)
        layout.addStretch()
        mainLayout.addLayout(layout)

        mainLayout.addSpacing(15)

        # Channel 5
    	lChannel5 = QtGui.QLabel("<b><font size=\"5\">Channel 5</font></b>")
    	mainLayout.addWidget(lChannel5)
    	layout = QtGui.QHBoxLayout()
    	layout.addSpacing(15)
    	lValue = QtGui.QLabel("Value")
    	layout.addWidget(lValue)
    	layout.addSpacing(15)
    	self.posBarChannel5 = PositionBar()
        self.posBarChannel5.setMinimumWidth(250)
        self.posBarChannel5.setMinimumHeight(30)
        self.posBarChannel5.setMaximumHeight(30)
        self.posBarChannel5.setValue(0)
        self.posBarChannel5.setMoveable(False)
        layout.addWidget(self.posBarChannel5)
    	self.lcdChannel5Value = QtGui.QLCDNumber()
        self.lcdChannel5Value.setMaximumHeight(25)
        self.lcdChannel5Value.setSegmentStyle(QtGui.QLCDNumber.Filled)
        layout.addWidget(self.lcdChannel5Value)
        layout.addStretch()
        mainLayout.addLayout(layout)

        mainLayout.addStretch()
    	self.setLayout(mainLayout)