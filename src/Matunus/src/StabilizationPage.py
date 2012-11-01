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
from BoardSettings import BoardSettings


class StabilizationPage(QtGui.QWidget):
	def __init__(self, controllerManager=None):
		super(StabilizationPage, self).__init__()

		if controllerManager is None:
			raise Exception

		self.boardController = controllerManager
		self.boardController.board_settings_updated.connect(self._onBoardStatusUpdated)

		self._boardSettings = BoardSettings()

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
		self.spbRollKp = QtGui.QDoubleSpinBox()
		self.spbRollKp.setMinimumWidth(150)
		hLayout.addWidget(self.spbRollKp)
		self.spbRollKi = QtGui.QDoubleSpinBox()
		self.spbRollKi.setMinimumWidth(150)
		hLayout.addWidget(self.spbRollKi)
		self.spbRollKd = QtGui.QDoubleSpinBox()
		self.spbRollKd.setMinimumWidth(150)
		hLayout.addWidget(self.spbRollKd)
		self.spbILimitRoll = QtGui.QDoubleSpinBox()
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
		self.spbPitchKp = QtGui.QDoubleSpinBox()
		self.spbPitchKp.setMinimumWidth(150)
		hLayout.addWidget(self.spbPitchKp)
		self.spbPitchKi = QtGui.QDoubleSpinBox()
		self.spbPitchKi.setMinimumWidth(150)
		hLayout.addWidget(self.spbPitchKi)
		self.spbPitchKd = QtGui.QDoubleSpinBox()
		self.spbPitchKd.setMinimumWidth(150)
		hLayout.addWidget(self.spbPitchKd)
		self.spbILimitPitch = QtGui.QDoubleSpinBox()
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
		self.spbYawKp = QtGui.QDoubleSpinBox()
		self.spbYawKp.setMinimumWidth(150)
		hLayout.addWidget(self.spbYawKp)
		self.spbYawKi = QtGui.QDoubleSpinBox()
		self.spbYawKi.setMinimumWidth(150)
		hLayout.addWidget(self.spbYawKi)
		self.spbYawKd = QtGui.QDoubleSpinBox()
		self.spbYawKd.setMinimumWidth(150)
		hLayout.addWidget(self.spbYawKd)
		self.spbILimitYaw = QtGui.QDoubleSpinBox()
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
		self.spbRollQAngle = QtGui.QDoubleSpinBox()
		self.spbRollQAngle.setMinimumWidth(150)
		hLayout.addWidget(self.spbRollQAngle)
		self.spbRollQGyro = QtGui.QDoubleSpinBox()
		self.spbRollQGyro.setMinimumWidth(150)
		hLayout.addWidget(self.spbRollQGyro)
		self.spbRollRAngle = QtGui.QDoubleSpinBox()
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
		self.spbPitchQAngle = QtGui.QDoubleSpinBox()
		self.spbPitchQAngle.setMinimumWidth(150)
		hLayout.addWidget(self.spbPitchQAngle)
		self.spbPitchQGyro = QtGui.QDoubleSpinBox()
		self.spbPitchQGyro.setMinimumWidth(150)
		hLayout.addWidget(self.spbPitchQGyro)
		self.spbPitchRAngle = QtGui.QDoubleSpinBox()
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
		self.spbYawQAngle = QtGui.QDoubleSpinBox()
		self.spbYawQAngle.setMinimumWidth(150)
		hLayout.addWidget(self.spbYawQAngle)
		self.spbYawQGyro = QtGui.QDoubleSpinBox()
		self.spbYawQGyro.setMinimumWidth(150)
		hLayout.addWidget(self.spbYawQGyro)
		self.spbYawRAngle = QtGui.QDoubleSpinBox()
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
		self.spbStickAngleRoll = QtGui.QDoubleSpinBox()
		self.spbStickAngleRoll.setMinimumWidth(150)
		hLayout.addWidget(self.spbStickAngleRoll)
		self.spbStickAnglePitch = QtGui.QDoubleSpinBox()
		self.spbStickAnglePitch.setMinimumWidth(150)
		hLayout.addWidget(self.spbStickAnglePitch)
		self.spbStickAngleYaw = QtGui.QDoubleSpinBox()
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
		self.spbStickRateRoll = QtGui.QDoubleSpinBox()
		self.spbStickRateRoll.setMinimumWidth(150)
		hLayout.addWidget(self.spbStickRateRoll)
		self.spbStickRatePitch = QtGui.QDoubleSpinBox()
		self.spbStickRatePitch.setMinimumWidth(150)
		hLayout.addWidget(self.spbStickRatePitch)
		self.spbStickRateYaw = QtGui.QDoubleSpinBox()
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
		self.spbStickMaxRateAttRoll = QtGui.QDoubleSpinBox()
		self.spbStickMaxRateAttRoll.setMinimumWidth(150)
		hLayout.addWidget(self.spbStickMaxRateAttRoll)
		self.spbStickMaxRateAttPitch = QtGui.QDoubleSpinBox()
		self.spbStickMaxRateAttPitch.setMinimumWidth(150)
		hLayout.addWidget(self.spbStickMaxRateAttPitch)
		self.spbStickMaxRateAttYaw = QtGui.QDoubleSpinBox()
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
		self.saveButton.clicked.connect(self._onSaveButtonClicked)
		hLayout.addWidget(self.saveButton)
		self.applyButton = QtGui.QPushButton("Apply")
		self.apllyButton.clicked.connect(self._onApplyButtonClicked)
		hLayout.addWidget(self.applyButton)
		
		mainLayout.addLayout(hLayout)

		# Set main layout
		self.setLayout(mainLayout)

	def _onApplyButtonClicked(self):
		pass

	def _onSaveButtonClicked(self):
		# TODO: Write all settings to the controller
		pass

	def _onBoardStatusUpdated(self):
		pass

	def _updateValues(self):
		self.spbRollKp.setValue()