#!/usr/bin/env python

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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

__author__ = 'Sebastian Ruml'

try:
    from PyQt4 import QtGui
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)

from StatusLabel import StatusLabel
from UavStatusGadget import UavStatusGadget


class InfoPanel(QtGui.QWidget):
	def __init__(self, comPorts, boardController):
		super(InfoPanel, self).__init__()

		self.comPorts = comPorts
		self.boardController = boardController

		self.setMinimumWidth(320)
		self.setMaximumWidth(320)

		# Create the UI
		self._createUi()

		self.systemStatus.setText("-")
		self.systemMessage.setText("-")

	def _createUi(self):
		self.mainLayout = QtGui.QVBoxLayout()

		label = QtGui.QLabel("<b><font size=\"5\" color='black'>Orca Information</font></b>")
		self.mainLayout.addWidget(label)
		self.mainLayout.addSpacing(5)
		
		# Layout for the connection controls
		layoutConnection = QtGui.QHBoxLayout()
		self.mainLayout.addLayout(layoutConnection)

		# Connect button
		self.bConnect = QtGui.QPushButton("Connect")
		self.bConnect.clicked.connect(self._onbConnectClicked)
		layoutConnection.addWidget(self.bConnect)
		layoutConnection.addSpacing(120)

		# ComboBox for the com ports
		self.cbComPort = QtGui.QComboBox()
		for comPort in self.comPorts:
			self.cbComPort.addItem(comPort)
		self.cbComPort.currentIndexChanged.connect(self._onComPortChanged)
		if len(self.comPorts) > 0:
			self.boardController.set_serial_port(self.comPorts[0])
		layoutConnection.addWidget(self.cbComPort)
		self.mainLayout.addSpacing(20)

		label = QtGui.QLabel("<b><font color='black'>System.Status</font></b>")
		self.mainLayout.addWidget(label)

		# System status label
		self.systemStatus = StatusLabel()
		self.mainLayout.addWidget(self.systemStatus)

		label = QtGui.QLabel("<b><font color='black'>System.Message</font></b>")
		self.mainLayout.addWidget(label)

		# System message label
		self.systemMessage = StatusLabel()
		self.mainLayout.addWidget(self.systemMessage)

		# UAV status overview
		self.uavStatusGadget = UavStatusGadget()
		self.uavStatusGadget.setMinimumWidth(300)
		self.uavStatusGadget.setMinimumHeight(250)
		self.uavStatusGadget.setLinkStatus(False)
		self.mainLayout.addWidget(self.uavStatusGadget)

		self.mainLayout.addStretch()
		self.setLayout(self.mainLayout)

	def _onComPortChanged(self):
		self.boardController.set_serial_port(self.comPorts[self.cbComPort.currentIndex()])

	def _onbConnectClicked(self):
		if not self.boardController.connected():
			self.cbComPort.setEnabled(False)
			self.uavStatusGadget.setLinkStatus(True)
			self.bConnect.setText("Disconnect")
		else:
			self.cbComPort.setEnabled(True)
			self.uavStatusGadget.setLinkStatus(False)
			self.bConnect.setText("Connect")

	def paintEvent(self, e):
		qp = QtGui.QPainter()
		qp.begin(self)
		self._drawBackground(qp)
		qp.end()

	def _drawBackground(self, qp):
		size = self.size()
		width = size.width()
		height = size.height()

		# Draw the background
		qp.setBrush(QtGui.QColor("#878486"))
		qp.drawRect(1, 1, width, height)

		# Draw frame
		qp.setPen(QtGui.QColor(0, 0, 0))
		#qp.setBrush(QtGui.QColor(0, 0, 0))
		qp.drawRect(0,0,width-1,height-1)

	def setSystemStatus(self, text):
		self.systemStatus.setText(text)

	def setSystemMessage(self, text):
		self.systemMessage.setText(text)