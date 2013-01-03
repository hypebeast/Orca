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
	from PyQt4 import QtGui, QtCore
except ImportError:
	print "No PyQt found!"
	import sys
	sys.exit(2)

from HeadingLabel import HeadingLabel
from BoardSettings import FmuSettings


class StabilizationPage(QtGui.QWidget):
	def __init__(self, controllerManager=None):
		super(StabilizationPage, self).__init__()

		if controllerManager is None:
			raise Exception

		self.fmuController = controllerManager
		self.fmuController.board_settings_updated.connect(self._onBoardSettingsUpdated)

		self._fmuSettings = FmuSettings()

		# Create the UI
		self._createUi()

	def _createUi(self):
		"""Initializes the UI."""
		mainLayout = QtGui.QVBoxLayout()

		# Stabilization Coefficients
		groupBox = QtGui.QGroupBox("Rate Stabilization (Inner Loop)")
		groupBoxLayout = QtGui.QVBoxLayout()
		groupBox.setLayout(groupBoxLayout)
		
		# Headers
		hLayout = QtGui.QHBoxLayout()
		hLayout.addSpacing(250)
		header = HeadingLabel()
		header.setTextAlign(QtCore.Qt.AlignHCenter)
		header.setText("Proportional")
		header.setMinimumWidth(100)
		hLayout.addWidget(header)
		hLayout.addSpacing(50)
		header = HeadingLabel()
		header.setTextAlign(QtCore.Qt.AlignHCenter)
		header.setText("Integral")
		header.setMinimumWidth(100)
		hLayout.addWidget(header)
		hLayout.addSpacing(50)
		header = HeadingLabel()
		header.setTextAlign(QtCore.Qt.AlignHCenter)
		header.setText("Derivative")
		header.setMinimumWidth(100)
		hLayout.addWidget(header)
		hLayout.addSpacing(50)
		header = HeadingLabel()
		header.setTextAlign(QtCore.Qt.AlignHCenter)
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
		hLayout.addWidget(self.spbRollKp)
		self.spbRollKi = QtGui.QDoubleSpinBox()
		self.spbRollKi.setDecimals(5)
		self.spbRollKi.setMinimum(0)
		self.spbRollKi.setMaximum(50)
		self.spbRollKi.setMinimumWidth(150)
		hLayout.addWidget(self.spbRollKi)
		self.spbRollKd = QtGui.QDoubleSpinBox()
		self.spbRollKd.setDecimals(5)
		self.spbRollKd.setMinimum(0)
		self.spbRollKd.setMaximum(50)
		self.spbRollKd.setMinimumWidth(150)
		hLayout.addWidget(self.spbRollKd)
		self.spbRollILimit = QtGui.QDoubleSpinBox()
		self.spbRollILimit.setDecimals(5)
		self.spbRollILimit.setMinimum(0)
		self.spbRollILimit.setMaximum(50)
		self.spbRollILimit.setMinimumWidth(150)
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

		# DCM Constants
		groupBox = QtGui.QGroupBox("Attitude Stabilization (Outer Loop)")
		groupBoxLayout = QtGui.QVBoxLayout()
		groupBox.setLayout(groupBoxLayout)

		mainLayout.addWidget(groupBox)      

		# Headers
		hLayout = QtGui.QHBoxLayout()
		hLayout.addSpacing(250)
		header = HeadingLabel()
		header.setTextAlign(QtCore.Qt.AlignHCenter)
		header.setText("Proportional")
		header.setMinimumWidth(100)
		hLayout.addWidget(header)
		hLayout.addSpacing(50)
		header = HeadingLabel()
		header.setText("Integral")
		header.setTextAlign(QtCore.Qt.AlignHCenter)
		header.setMinimumWidth(100)
		hLayout.addWidget(header)
		hLayout.addStretch()
		groupBoxLayout.addLayout(hLayout)  

		# DCM Roll/Pitch
		hLayout = QtGui.QHBoxLayout()
		hLayout.addSpacing(50)
		label = QtGui.QLabel("Roll/Pitch")
		hLayout.addWidget(label)
		hLayout.addSpacing(120)
		self.spbDcmRollPitchPFactor = QtGui.QDoubleSpinBox()
		self.spbDcmRollPitchPFactor.setDecimals(5)
		self.spbDcmRollPitchPFactor.setMinimum(0)
		self.spbDcmRollPitchPFactor.setMaximum(50)
		self.spbDcmRollPitchPFactor.setMinimumWidth(150)
		hLayout.addWidget(self.spbDcmRollPitchPFactor)
		self.spbDcmRollPitchIFactor = QtGui.QDoubleSpinBox()
		self.spbDcmRollPitchIFactor.setDecimals(5)
		self.spbDcmRollPitchIFactor.setMinimum(0)
		self.spbDcmRollPitchIFactor.setMaximum(50)
		self.spbDcmRollPitchIFactor.setMinimumWidth(150)
		hLayout.addWidget(self.spbDcmRollPitchIFactor)
		hLayout.addStretch()
		groupBoxLayout.addLayout(hLayout)

		# DCM Yaw
		hLayout = QtGui.QHBoxLayout()
		hLayout.addSpacing(50)
		label = QtGui.QLabel("Yaw")
		hLayout.addWidget(label)
		hLayout.addSpacing(145)
		self.spbDcmYawPFactor = QtGui.QDoubleSpinBox()
		self.spbDcmYawPFactor.setDecimals(5)
		self.spbDcmYawPFactor.setMinimum(0)
		self.spbDcmYawPFactor.setMaximum(50)
		self.spbDcmYawPFactor.setMinimumWidth(150)
		hLayout.addWidget(self.spbDcmYawPFactor)
		self.spbDcmYawIFactor = QtGui.QDoubleSpinBox()
		self.spbDcmYawIFactor.setDecimals(5)
		self.spbDcmYawIFactor.setMinimum(0)
		self.spbDcmYawIFactor.setMaximum(50)
		self.spbDcmYawIFactor.setMinimumWidth(150)
		hLayout.addWidget(self.spbDcmYawIFactor)
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
		header.setTextAlign(QtCore.Qt.AlignHCenter)
		header.setText("Roll")
		header.setMinimumWidth(100)
		hLayout.addWidget(header)
		hLayout.addSpacing(50)
		header = HeadingLabel()
		header.setTextAlign(QtCore.Qt.AlignHCenter)
		header.setText("Pitch")
		header.setMinimumWidth(100)
		hLayout.addWidget(header)
		hLayout.addSpacing(50)
		header = HeadingLabel()
		header.setTextAlign(QtCore.Qt.AlignHCenter)
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
		self.spbPitchKp.valueChanged.connect(self._onValueChanged)
		self.spbPitchKi.valueChanged.connect(self._onValueChanged)
		self.spbPitchKd.valueChanged.connect(self._onValueChanged)
		self.spbILimitPitch.valueChanged.connect(self._onValueChanged)
		self.spbYawKp.valueChanged.connect(self._onValueChanged)
		self.spbYawKi.valueChanged.connect(self._onValueChanged)
		self.spbYawKd.valueChanged.connect(self._onValueChanged)
		self.spbILimitYaw.valueChanged.connect(self._onValueChanged)
		self.spbDcmRollPitchPFactor.valueChanged.connect(self._onDcmRollPitchPFactorChanged)
		self.spbDcmRollPitchIFactor.valueChanged.connect(self._onDcmRollPitchIFactorChanged)
		self.spbDcmYawPFactor.valueChanged.connect(self._onValueChanged)
		self.spbDcmYawIFactor.valueChanged.connect(self._onValueChanged)

	def _disconnectSignals(self):
		self.spbRollKp.valueChanged.disconnect(self._onPidRollKpChanged)
		self.spbRollKi.valueChanged.disconnect(self._onPidRollKiChanged)
		self.spbRollKd.valueChanged.disconnect(self._onPidRollKdChanged)
		self.spbRollILimit.valueChanged.disconnect(self._onPidRollILimitChanged)
		self.spbPitchKp.valueChanged.disconnect(self._onValueChanged)
		self.spbPitchKi.valueChanged.disconnect(self._onValueChanged)
		self.spbPitchKd.valueChanged.disconnect(self._onValueChanged)
		self.spbILimitPitch.valueChanged.disconnect(self._onValueChanged)
		self.spbYawKp.valueChanged.disconnect(self._onValueChanged)
		self.spbYawKi.valueChanged.disconnect(self._onValueChanged)
		self.spbYawKd.valueChanged.disconnect(self._onValueChanged)
		self.spbILimitYaw.valueChanged.disconnect(self._onValueChanged)
		self.spbDcmRollPitchPFactor.valueChanged.disconnect(self._onDcmRollPitchPFactorChanged)
		self.spbDcmRollPitchIFactor.valueChanged.disconnect(self._onDcmRollPitchIFactorChanged)
		self.spbDcmYawPFactor.valueChanged.disconnect(self._onValueChanged)
		self.spbDcmYawIFactor.valueChanged.disconnect(self._onValueChanged)

	def _onApplyButtonClicked(self):
		# Send settings to the controller
		self._applyValues()

		# Disable apply button
		self.applyButton.setEnabled(False)

	def _applyValues(self):
		"""Writes all settings values to the controller."""
		# PID Roll settings
		self.fmuController.setPIDRollCoefficients(self.spbRollKp.value(),
												self.spbRollKi.value(),
												self.spbRollKd.value(),
												self.spbRollILimit.value())

		# PID Pitch settings
		self.fmuController.setPIDPitchCoefficients(self.spbPitchKp.value(),
												self.spbPitchKi.value(),
												self.spbPitchKd.value(),
												self.spbPitchILimit.value())

		# PID Yaw settings
		self.fmuController.setPIDYawCoefficients(self.spbYawKp.value(),
												self.spbYawKi.value(),
												self.spbYawKd.value(),
												self.spbILimitYaw.value())

		# DCM Roll settings
		self.fmuController.setDcmRollCoefficients(self.spbDcmRollPitchPFactor.value(),
												self.spbDcmRollPitchIFactor.value())

		# DCM Pitch settings
		self.fmuController.setDcmPitchCoefficients(self.spbDcmRollPitchPFactor.value(),
												self.spbDcmRollPitchIFactor.value())

		# DCM Yaw settings
		self.fmuController.setDcmYawCoefficients(self.spbDcmYawPFactor.value(),
												self.spbDcmYawIFactor.value())

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
		self.fmuController.saveBoardSettings()

	def _onBoardSettingsUpdated(self):
		"""Called when the board settings are updated."""
		self._updateValues()

	def _updateValues(self):
		"""Update GUI elements."""
		# Disable events
		self._disconnectSignals()
		
		self.spbRollKp.setValue(self._fmuSettings.pidRollPFactor)
		self.spbRollKi.setValue(self._fmuSettings.pidRollIFactor)
		self.spbRollKd.setValue(self._fmuSettings.pidRollDFactor)
		self.spbRollILimit.setValue(self._fmuSettings.pidRollILimit)
		self.spbPitchKp.setValue(self._fmuSettings.pidPitchPFactor)
		self.spbPitchKi.setValue(self._fmuSettings.pidPitchIFactor)
		self.spbPitchKd.setValue(self._fmuSettings.pidPitchDFactor)
		self.spbILimitPitch.setValue(self._fmuSettings.pidPitchILimit)
		self.spbYawKp.setValue(self._fmuSettings.pidYawPFactor)
		self.spbYawKi.setValue(self._fmuSettings.pidYawIFactor)
		self.spbYawKd.setValue(self._fmuSettings.pidYawDFactor)
		self.spbILimitYaw.setValue(self._fmuSettings.pidYawILimit)

		self.spbDcmRollPitchPFactor.setValue(self._fmuSettings.dcmRollPFactor)
		self.spbDcmRollPitchIFactor.setValue(self._fmuSettings.dcmRollIFactor)
		self.spbDcmYawPFactor.setValue(self._fmuSettings.dcmYawPFactor)
		self.spbDcmYawIFactor.setValue(self._fmuSettings.dcmYawIFactor)

		# Re-enable events
		self._connectSignals()

	def _onValueChanged(self, value):
		self._enableButtons()

	def _onPidRollKpChanged(self, value):
		self._enableButtons()

	def _onPidRollKiChanged(self, value):
		self._enableButtons()

	def _onPidRollKdChanged(self, value):
		self._enableButtons()

	def _onPidRollILimitChanged(self, value):
		self._enableButtons()

	def _onDcmRollPitchPFactorChanged(self, value):
		self._enableButtons()

	def _onDcmRollPitchIFactorChanged(self, value):
		self._enableButtons()

	def _onKalmanRollRAngleChanged(self, value):
		self._enableButtons()

	def _enableButtons(self):
		# Settings were changed; enable save and apply buttons
		self.saveButton.setEnabled(True)
		self.applyButton.setEnabled(True)

	def _onbRestoreFactoryClicked(self):
		self._fmuSettings.restoreStabilizationFactorySettings()
		self._updateValues()
		self._enableButtons()

	def _onbReloadFromBoard(self):
		self.fmuController.updateBoardSettings()