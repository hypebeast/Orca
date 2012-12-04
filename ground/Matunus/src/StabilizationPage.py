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
		self.boardController.board_settings_updated.connect(self._onBoardSettingsUpdated)

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
		self.spbRollKp.setDecimals(5)
		self.spbRollKp.setMinimum(0)
		self.spbRollKp.setMaximum(50)
		self.spbRollKp.setMinimumWidth(150)
		self.spbRollKp.valueChanged.connect(self._onPidRollKpChanged)
		hLayout.addWidget(self.spbRollKp)
		self.spbRollKi = QtGui.QDoubleSpinBox()
		self.spbRollKi.setDecimals(5)
		self.spbRollKi.setMinimum(0)
		self.spbRollKi.setMaximum(50)
		self.spbRollKi.setMinimumWidth(150)
		self.spbRollKi.valueChanged.connect(self._onPidRollKiChanged)
		hLayout.addWidget(self.spbRollKi)
		self.spbRollKd = QtGui.QDoubleSpinBox()
		self.spbRollKd.setDecimals(5)
		self.spbRollKd.setMinimum(0)
		self.spbRollKd.setMaximum(50)
		self.spbRollKd.setMinimumWidth(150)
		self.spbRollKd.valueChanged.connect(self._onPidRollKdChanged)
		hLayout.addWidget(self.spbRollKd)
		self.spbRollILimit = QtGui.QDoubleSpinBox()
		self.spbRollILimit.setDecimals(5)
		self.spbRollILimit.setMinimum(0)
		self.spbRollILimit.setMaximum(50)
		self.spbRollILimit.setMinimumWidth(150)
		self.spbRollILimit.valueChanged.connect(self._onPidRollILimitChanged)
		hLayout.addWidget(self.spbRollILimit)
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

		# Kalman Roll constants
		hLayout = QtGui.QHBoxLayout()
		hLayout.addSpacing(50)
		label = QtGui.QLabel("Roll")
		hLayout.addWidget(label)
		hLayout.addSpacing(150)
		self.spbRollQAngle = QtGui.QDoubleSpinBox()
		self.spbRollQAngle.setDecimals(5)
		self.spbRollQAngle.setMinimum(0)
		self.spbRollQAngle.setMaximum(50)
		self.spbRollQAngle.setMinimumWidth(150)
		self.spbRollQAngle.valueChanged.connect(self._onKalmanRollQAngleChanged)
		hLayout.addWidget(self.spbRollQAngle)
		self.spbRollQGyro = QtGui.QDoubleSpinBox()
		self.spbRollQGyro.setDecimals(5)
		self.spbRollQGyro.setMinimum(0)
		self.spbRollQGyro.setMaximum(50)
		self.spbRollQGyro.setMinimumWidth(150)
		self.spbRollQGyro.valueChanged.connect(self._onKalmanRollQGyroChanged)
		hLayout.addWidget(self.spbRollQGyro)
		self.spbRollRAngle = QtGui.QDoubleSpinBox()
		self.spbRollRAngle.setDecimals(5)
		self.spbRollRAngle.setMinimum(0)
		self.spbRollRAngle.setMaximum(50)
		self.spbRollRAngle.setMinimumWidth(150)
		self.spbRollRAngle.valueChanged.connect(self._onKalmanRollRAngleChanged)
		hLayout.addWidget(self.spbRollRAngle)
		hLayout.addStretch()
		groupBoxLayout.addLayout(hLayout)

		# Kalman Pitch constants
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

		# Kalman Yaw constants
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

		# Bottom buttons
		hLayout = QtGui.QHBoxLayout()
		self.bRestoreFactory = QtGui.QPushButton("Restore Factory Settings")
		self.bRestoreFactory.clicked.connect(self._onbRestoreFactoryClicked)
		hLayout.addWidget(self.bRestoreFactory)
		self.bReloadFromBoard = QtGui.QPushButton("Reload data from board")
		self.bReloadFromBoard.clicked.connect(self._onbReloadFromBoard)
		hLayout.addWidget(self.bReloadFromBoard)
		hLayout.addStretch()
		self.saveButton = QtGui.QPushButton("Save")
		self.saveButton.setEnabled(False)
		self.saveButton.clicked.connect(self._onSaveButtonClicked)
		hLayout.addWidget(self.saveButton)
		self.applyButton = QtGui.QPushButton("Apply")
		self.applyButton.setEnabled(False)
		self.applyButton.clicked.connect(self._onApplyButtonClicked)
		hLayout.addWidget(self.applyButton)
		
		mainLayout.addLayout(hLayout)

		# Set main layout
		self.setLayout(mainLayout)

		# Connect all signals
		self._connectSignals()

	def _connectSignals(self):
		self.spbRollKp.valueChanged.connect(self._onPidRollKpChanged)
		self.spbRollKi.valueChanged.connect(self._onPidRollKiChanged)
		self.spbRollKd.valueChanged.connect(self._onPidRollKdChanged)
		self.spbRollILimit.valueChanged.connect(self._onPidRollILimitChanged)
		self.spbRollQAngle.valueChanged.connect(self._onKalmanRollQAngleChanged)
		self.spbRollQGyro.valueChanged.connect(self._onKalmanRollQGyroChanged)
		self.spbRollRAngle.valueChanged.connect(self._onKalmanRollRAngleChanged)

	def _disconnectSignals(self):
		self.spbRollKp.valueChanged.disconnect(self._onPidRollKpChanged)
		self.spbRollKi.valueChanged.disconnect(self._onPidRollKiChanged)
		self.spbRollKd.valueChanged.disconnect(self._onPidRollKdChanged)
		self.spbRollILimit.valueChanged.disconnect(self._onPidRollILimitChanged)
		self.spbRollQAngle.valueChanged.disconnect(self._onKalmanRollQAngleChanged)
		self.spbRollQGyro.valueChanged.disconnect(self._onKalmanRollQGyroChanged)
		self.spbRollRAngle.valueChanged.disconnect(self._onKalmanRollRAngleChanged)

	def _onApplyButtonClicked(self):
		# Send settings to the controller
		self._applyValues()

		# Disable apply button
		self.applyButton.setEnabled(False)

	def _applyValues(self):
		"""Writes all settings values to the controller."""
		# PID roll settings
		self.boardController.setRollPIDCoefficients(self.spbRollKp.value(),
			self.spbRollKi.value(), self.spbRollKd.value(),
			self.spbRollILimit.value())

		# Kalman roll settings
		self.boardController.setRollKalmanConstants(self.spbRollQAngle.value(),
			self.spbRollQGyro.value(), self.spbRollRAngle.value())

		# Disable apply button
		self.applyButton.setEnabled(False)

	def _onSaveButtonClicked(self):
		# Send settings to the controller
		self._applyValues()

		# Save settings
		self._saveValues()

		# Disable save and apply button
		self.saveButton.setEnabled(False)
		self.applyButton.setEnabled(False)

	def _saveValues(self):
		"""Saves the settings on the controller."""
		# Save settings
		self.boardController.saveBoardSettings()

	def _onBoardSettingsUpdated(self):
		"""Called when the board settings are updated."""
		self._updateValues()

	def _updateValues(self):
		"""Update GUI elements."""
		# Disable events
		self._disconnectSignals()
		
		self.spbRollKp.setValue(self._boardSettings.pidRollPFactor)
		self.spbRollKi.setValue(self._boardSettings.pidRollIFactor)
		self.spbRollKd.setValue(self._boardSettings.pidRollDFactor)
		self.spbRollILimit.setValue(self._boardSettings.pidRollILimit)

		self.spbRollQAngle.setValue(self._boardSettings.kalmanRollQAngle)
		self.spbRollQGyro.setValue(self._boardSettings.kalmanRollQGyro)
		self.spbRollRAngle.setValue(self._boardSettings.kalmanRollRAngle)

		# Re-enable events
		self._connectSignals()

	def _onPidRollKpChanged(self, value):
		self._enableButtons()

	def _onPidRollKiChanged(self, value):
		self._enableButtons()

	def _onPidRollKdChanged(self, value):
		self._enableButtons()

	def _onPidRollILimitChanged(self, value):
		self._enableButtons()

	def _onKalmanRollQAngleChanged(self, value):
		self._enableButtons()

	def _onKalmanRollQGyroChanged(self, value):
		self._enableButtons()

	def _onKalmanRollRAngleChanged(self, value):
		self._enableButtons()

	def _enableButtons(self):
		# Settings were changed; enable save and apply buttons
		self.saveButton.setEnabled(True)
		self.applyButton.setEnabled(True)

	def _onbRestoreFactoryClicked(self):
		self._boardSettings.restoreStabilizationFactorySettings()
		self._updateValues()
		self._enableButtons()

	def _onbReloadFromBoard(self):
		self.boardController.updateBoardSettings()