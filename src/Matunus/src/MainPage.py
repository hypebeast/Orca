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

from StatusLabel import StatusLabel


class MainPage(QtGui.QWidget):
    def __init__(self):
        super(MainPage, self).__init__()

        self.createUi()

    def createUi(self):
        mainLayout = QtGui.QVBoxLayout()

        # Status group box
        groupBoxStatus = QtGui.QGroupBox("Status")
        vLayout = QtGui.QVBoxLayout()
        systemStatusLayout = QtGui.QHBoxLayout()
        lSystemStatus = QtGui.QLabel("System.Status:")
        systemStatusLayout.addWidget(lSystemStatus)
        systemStatusLayout.insertSpacing(1, 20)
        self.systemStatus = StatusLabel()
        self.systemStatus.setText("Off")
        self.systemStatus.setMaximumHeight(40)
        self.systemStatus.setMaximumWidth(80)
        systemStatusLayout.addWidget(self.systemStatus)
        systemStatusLayout.insertSpacing(3, 30)
        lSystemMessage = QtGui.QLabel("System.Message:")
        systemStatusLayout.addWidget(lSystemMessage)
        systemStatusLayout.insertSpacing(5, 20)
        self.systemMessage = StatusLabel()
        self.systemMessage.setText("None")
        self.systemMessage.setMaximumHeight(40)
        self.systemMessage.setMinimumWidth(300)
        systemStatusLayout.addWidget(self.systemMessage)
        systemStatusLayout.addStretch()
        vLayout.addItem(systemStatusLayout)

        vLayout.addStretch()
        groupBoxStatus.setLayout(vLayout)

        mainLayout.addWidget(groupBoxStatus)
        mainLayout.addStretch()
        self.setLayout(mainLayout)

    def setSystemStatus(self, text):
        self.systemStatus.setText(text)

    def setSystemMessage(self, message):
        self.systemMessage.setText(message)
