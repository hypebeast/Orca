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

from EngineControlWidget import EngineControlWidget
from EngineControlWidget import Engines


class EnginePage(QtGui.QWidget):
    def __init__(self, serial=None):
        super(EnginePage, self).__init__()

        if serial is None:
        	raise Exception()

        self.serial_connection = serial

        # Create the GUI
        self.create_ui()

    def create_ui(self):
    	mainLayout = QtGui.QVBoxLayout()

        # Engine 1 (left)
        groupBox1 = QtGui.QGroupBox("Engine 1 (Left)")
        groupBox1.setMinimumHeight(70)
        self.engine_left_control = EngineControlWidget(self.serial_connection, Engines.EngineLeft)
        hBox = QtGui.QHBoxLayout()
        hBox.setMargin(2)
        hBox.addWidget(self.engine_left_control)
        groupBox1.setLayout(hBox)
        mainLayout.addWidget(groupBox1)

        # Engine 2 (right)
        groupBox2 = QtGui.QGroupBox("Engine 2 (Right)")
        groupBox2.setMinimumHeight(70)
        self.engine_right_control = EngineControlWidget(self.serial_connection, Engines.EngineRight)
        hBox = QtGui.QHBoxLayout()
        hBox.setMargin(2)
        hBox.addWidget(self.engine_right_control)
        groupBox2.setLayout(hBox)
        mainLayout.addWidget(groupBox2)

        # Engine 3 (tail)
        groupBox3 = QtGui.QGroupBox("Engine 3 (Tail)")
        groupBox3.setMinimumHeight(70)
        self.engine_tail_control = EngineControlWidget(self.serial_connection, Engines.EngineTail)
        hBox = QtGui.QHBoxLayout()
        hBox.setMargin(2)
        hBox.addWidget(self.engine_tail_control)
        groupBox3.setLayout(hBox)
        mainLayout.addWidget(groupBox3)

        # All engines
        groupBox4 = QtGui.QGroupBox("All Engines")
        groupBox4.setMinimumHeight(150)
        mainLayout.addWidget(groupBox4)

        # Configuration
        groupBox5 = QtGui.QGroupBox("Configuration")
        groupBox5.setMinimumHeight(100)
        mainLayout.addWidget(groupBox5)
        mainLayout.addStretch()

        self.setLayout(mainLayout)
