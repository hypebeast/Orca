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

from HeadingLabel import HeadingLabel

class StabilizationPage(QtGui.QWidget):
	def __init__(self):
		super(StabilizationPage, self).__init__()

		self._createUi()

	def _createUi(self):
		"""Initializes the UI."""
		mainLayout = QtGui.QVBoxLayout()

		# Stabilization Coefficients
		groupBox = QtGui.QGroupBox("Stabilization Coefficients")
		groupBoxLayout = QtGui.QVBoxLayout()
		groupBox.setLayout(groupBoxLayout)
		
		# Headers
		hLayout = QtGui.QHBoxLayout()
		hLayout.addSpacing(250)
		header = HeadingLabel()
		header.setText("Kp")
		header.setMinimumWidth(100)
		hLayout.addWidget(header)
		hLayout.addSpacing(50)
		header = HeadingLabel()
		header.setText("Ki")
		header.setMinimumWidth(100)
		hLayout.addWidget(header)
		hLayout.addSpacing(50)
		header = HeadingLabel()
		header.setText("Kd")
		header.setMinimumWidth(100)
		hLayout.addWidget(header)
		hLayout.addSpacing(50)
		header = HeadingLabel()
		header.setText("ILimit")
		header.setMinimumWidth(100)
		hLayout.addWidget(header)
		hLayout.addStretch()
		groupBoxLayout.addLayout(hLayout)
		
		# Roll coefficients
		hLayout = QtGui.QHBoxLayout()
		hLayout.addSpacing(50)
		label = QtGui.QLabel("Roll")
		hLayout.addWidget(label)
		hLayout.addSpacing(150)
		self.spbRollKp = QtGui.QSpinBox()
		self.spbRollKp.setMinimumWidth(150)
		hLayout.addWidget(self.spbRollKp)
		self.spbRollKi = QtGui.QSpinBox()
		self.spbRollKi.setMinimumWidth(150)
		hLayout.addWidget(self.spbRollKi)
		self.spbRollKd = QtGui.QSpinBox()
		self.spbRollKd.setMinimumWidth(150)
		hLayout.addWidget(self.spbRollKd)
		self.spbILimitRoll = QtGui.QSpinBox()
		self.spbILimitRoll.setMinimumWidth(150)
		hLayout.addWidget(self.spbILimitRoll)
		hLayout.addStretch()
		groupBoxLayout.addLayout(hLayout)
		
		# Pitch coefficients
		hLayout = QtGui.QHBoxLayout()
		hLayout.addSpacing(50)
		label = QtGui.QLabel("Pitch")
		hLayout.addWidget(label)
		hLayout.addSpacing(145)
		self.spbPitchKp = QtGui.QSpinBox()
		self.spbPitchKp.setMinimumWidth(150)
		hLayout.addWidget(self.spbPitchKp)
		self.spbPitchKi = QtGui.QSpinBox()
		self.spbPitchKi.setMinimumWidth(150)
		hLayout.addWidget(self.spbPitchKi)
		self.spbPitchKd = QtGui.QSpinBox()
		self.spbPitchKd.setMinimumWidth(150)
		hLayout.addWidget(self.spbPitchKd)
		self.spbILimitPitch = QtGui.QSpinBox()
		self.spbILimitPitch.setMinimumWidth(150)
		hLayout.addWidget(self.spbILimitPitch)
		hLayout.addStretch()
		groupBoxLayout.addLayout(hLayout)

		# Yaw coefficients
		hLayout = QtGui.QHBoxLayout()
		hLayout.addSpacing(50)
		label = QtGui.QLabel("Yaw")
		hLayout.addWidget(label)
		hLayout.addSpacing(148)
		self.spbYawKp = QtGui.QSpinBox()
		self.spbYawKp.setMinimumWidth(150)
		hLayout.addWidget(self.spbYawKp)
		self.spbYawKi = QtGui.QSpinBox()
		self.spbYawKi.setMinimumWidth(150)
		hLayout.addWidget(self.spbYawKi)
		self.spbYawKd = QtGui.QSpinBox()
		self.spbYawKd.setMinimumWidth(150)
		hLayout.addWidget(self.spbYawKd)
		self.spbILimitYaw = QtGui.QSpinBox()
		self.spbILimitYaw.setMinimumWidth(150)
		hLayout.addWidget(self.spbILimitYaw)
		hLayout.addStretch()
		groupBoxLayout.addLayout(hLayout)
		
		groupBoxLayout.addSpacing(100)
		mainLayout.addWidget(groupBox)

		# Kalman Constants
		groupBox = QtGui.QGroupBox("Kalman Constants")
		groupBoxLayout = QtGui.QVBoxLayout()
		groupBox.setLayout(groupBoxLayout)

		mainLayout.addWidget(groupBox)      

		# Headers
		hLayout = QtGui.QHBoxLayout()
		hLayout.addSpacing(250)
		header = HeadingLabel()
		header.setText("Q_Angle")
		header.setMinimumWidth(100)
		hLayout.addWidget(header)
		hLayout.addSpacing(50)
		header = HeadingLabel()
		header.setText("Q_Gyro")
		header.setMinimumWidth(100)
		hLayout.addWidget(header)
		hLayout.addSpacing(50)
		header = HeadingLabel()
		header.setText("R_Angle")
		header.setMinimumWidth(100)
		hLayout.addWidget(header)
		hLayout.addStretch()
		groupBoxLayout.addLayout(hLayout)  

		# Roll constants
		hLayout = QtGui.QHBoxLayout()
		hLayout.addSpacing(50)
		label = QtGui.QLabel("Roll")
		hLayout.addWidget(label)
		hLayout.addSpacing(150)
		self.spbRollQAngle = QtGui.QSpinBox()
		self.spbRollQAngle.setMinimumWidth(150)
		hLayout.addWidget(self.spbRollQAngle)
		self.spbRollQGyro = QtGui.QSpinBox()
		self.spbRollQGyro.setMinimumWidth(150)
		hLayout.addWidget(self.spbRollQGyro)
		self.spbRollRAngle = QtGui.QSpinBox()
		self.spbRollRAngle.setMinimumWidth(150)
		hLayout.addWidget(self.spbRollRAngle)
		hLayout.addStretch()
		groupBoxLayout.addLayout(hLayout)

		# Pitch constants
		hLayout = QtGui.QHBoxLayout()
		hLayout.addSpacing(50)
		label = QtGui.QLabel("Pitch")
		hLayout.addWidget(label)
		hLayout.addSpacing(145)
		self.spbPitchQAngle = QtGui.QSpinBox()
		self.spbPitchQAngle.setMinimumWidth(150)
		hLayout.addWidget(self.spbPitchQAngle)
		self.spbPitchQGyro = QtGui.QSpinBox()
		self.spbPitchQGyro.setMinimumWidth(150)
		hLayout.addWidget(self.spbPitchQGyro)
		self.spbPitchRAngle = QtGui.QSpinBox()
		self.spbPitchRAngle.setMinimumWidth(150)
		hLayout.addWidget(self.spbPitchRAngle)
		hLayout.addStretch()
		groupBoxLayout.addLayout(hLayout)

		# Yaw constants
		hLayout = QtGui.QHBoxLayout()
		hLayout.addSpacing(50)
		label = QtGui.QLabel("Yaw")
		hLayout.addWidget(label)
		hLayout.addSpacing(150)
		self.spbYawQAngle = QtGui.QSpinBox()
		self.spbYawQAngle.setMinimumWidth(150)
		hLayout.addWidget(self.spbYawQAngle)
		self.spbYawQGyro = QtGui.QSpinBox()
		self.spbYawQGyro.setMinimumWidth(150)
		hLayout.addWidget(self.spbYawQGyro)
		self.spbYawRAngle = QtGui.QSpinBox()
		self.spbYawRAngle.setMinimumWidth(150)
		hLayout.addWidget(self.spbYawRAngle)
		hLayout.addStretch()
		groupBoxLayout.addLayout(hLayout)
		groupBoxLayout.addSpacing(100)

		# Stick range and limits
		groupBox = QtGui.QGroupBox("Stick ranges and limits")
		groupBoxLayout = QtGui.QVBoxLayout()
		groupBox.setLayout(groupBoxLayout)

		mainLayout.addWidget(groupBox)

		# Headers
		hLayout = QtGui.QHBoxLayout()
		hLayout.addSpacing(300)
		header = HeadingLabel()
		header.setText("Roll")
		header.setMinimumWidth(100)
		hLayout.addWidget(header)
		hLayout.addSpacing(50)
		header = HeadingLabel()
		header.setText("Pitch")
		header.setMinimumWidth(100)
		hLayout.addWidget(header)
		hLayout.addSpacing(50)
		header = HeadingLabel()
		header.setText("Yaw")
		header.setMinimumWidth(100)
		hLayout.addWidget(header)
		hLayout.addStretch()
		groupBoxLayout.addLayout(hLayout)

		# Full stick angle
		hLayout = QtGui.QHBoxLayout()
		hLayout.addSpacing(50)
		label = QtGui.QLabel("Full Stick Angle (deg)")
		hLayout.addWidget(label)
		hLayout.addSpacing(120)
		self.spbStickAngleRoll = QtGui.QSpinBox()
		self.spbStickAngleRoll.setMinimumWidth(150)
		hLayout.addWidget(self.spbStickAngleRoll)
		self.spbStickAnglePitch = QtGui.QSpinBox()
		self.spbStickAnglePitch.setMinimumWidth(150)
		hLayout.addWidget(self.spbStickAnglePitch)
		self.spbStickAngleYaw = QtGui.QSpinBox()
		self.spbStickAngleYaw.setMinimumWidth(150)
		hLayout.addWidget(self.spbStickAngleYaw)
		hLayout.addStretch()
		groupBoxLayout.addLayout(hLayout)

		# Full stick rate
		hLayout = QtGui.QHBoxLayout()
		hLayout.addSpacing(50)
		label = QtGui.QLabel("Full Stick Rate (deg/s)")
		hLayout.addWidget(label)
		hLayout.addSpacing(115)
		self.spbStickRateRoll = QtGui.QSpinBox()
		self.spbStickRateRoll.setMinimumWidth(150)
		hLayout.addWidget(self.spbStickRateRoll)
		self.spbStickRatePitch = QtGui.QSpinBox()
		self.spbStickRatePitch.setMinimumWidth(150)
		hLayout.addWidget(self.spbStickRatePitch)
		self.spbStickRateYaw = QtGui.QSpinBox()
		self.spbStickRateYaw.setMinimumWidth(150)
		hLayout.addWidget(self.spbStickRateYaw)
		hLayout.addStretch()
		groupBoxLayout.addLayout(hLayout)

		# Maximum rate in attitude mode
		hLayout = QtGui.QHBoxLayout()
		hLayout.addSpacing(50)
		label = QtGui.QLabel("Maximum Rate in Attitude Mode (deg/s)")
		hLayout.addWidget(label)
		hLayout.addSpacing(30)
		self.spbStickMaxRateAttRoll = QtGui.QSpinBox()
		self.spbStickMaxRateAttRoll.setMinimumWidth(150)
		hLayout.addWidget(self.spbStickMaxRateAttRoll)
		self.spbStickMaxRateAttPitch = QtGui.QSpinBox()
		self.spbStickMaxRateAttPitch.setMinimumWidth(150)
		hLayout.addWidget(self.spbStickMaxRateAttPitch)
		self.spbStickMaxRateAttYaw = QtGui.QSpinBox()
		self.spbStickMaxRateAttYaw.setMinimumWidth(150)
		hLayout.addWidget(self.spbStickMaxRateAttYaw)
		hLayout.addStretch()
		groupBoxLayout.addLayout(hLayout)

		groupBoxLayout.addSpacing(100)

		mainLayout.addStretch()

		# Save and apply button
		hLayout = QtGui.QHBoxLayout()
		self.cbLiveUpdate = QtGui.QCheckBox("Update in real time")
		hLayout.addWidget(self.cbLiveUpdate)
		hLayout.addStretch()
		self.saveButton = QtGui.QPushButton("Save")
		hLayout.addWidget(self.saveButton)
		self.applyButton = QtGui.QPushButton("Apply")
		hLayout.addWidget(self.applyButton)
		

		mainLayout.addLayout(hLayout)

		# Set main layout
		self.setLayout(mainLayout)

