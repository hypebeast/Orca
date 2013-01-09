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
		gridLayout = QtGui.QGridLayout()
		groupBox.setLayout(gridLayout)
		mainLayout.addWidget(groupBox)
		gridLayout.setSpacing(4)
		gridLayout.setColumnMinimumWidth(0, 20)
		gridLayout.setColumnMinimumWidth(1, 80)
		gridLayout.setColumnMinimumWidth(2, 20)
		gridLayout.setColumnMinimumWidth(3, 100)
		gridLayout.setColumnMinimumWidth(4, 100)
		gridLayout.setColumnMinimumWidth(5, 100)
		gridLayout.setColumnMinimumWidth(6, 100)
		gridLayout.setColumnMinimumWidth(7, 150)
		gridLayout.setColumnStretch(0, 0)
		gridLayout.setColumnStretch(1, 1)
		gridLayout.setColumnStretch(2, 0)
		gridLayout.setColumnStretch(3, 1)
		gridLayout.setColumnStretch(4, 1)
		gridLayout.setColumnStretch(5, 1)
		gridLayout.setColumnStretch(6, 1)
		gridLayout.setColumnStretch(7, 2)
		# Add spacer items
		spacer = QtGui.QSpacerItem(20, 20)
		gridLayout.addItem(spacer, 0, 0)
		spacer = QtGui.QSpacerItem(20, 20)
		gridLayout.addItem(spacer, 0, 2)
		spacer = QtGui.QSpacerItem(150, 20)
		gridLayout.addItem(spacer, 0, 7)
		spacer = QtGui.QSpacerItem(50, 120)
		gridLayout.addItem(spacer, 8, 0)

		
		# Headers
		header = HeadingLabel()
		header.setTextAlign(QtCore.Qt.AlignHCenter)
		header.setText("Proportional")
		gridLayout.addWidget(header, 0, 3)
		header = HeadingLabel()
		header.setTextAlign(QtCore.Qt.AlignHCenter)
		header.setText("Integral")
		gridLayout.addWidget(header, 0, 4)
		header = HeadingLabel()
		header.setTextAlign(QtCore.Qt.AlignHCenter)
		header.setText("Derivative")
		gridLayout.addWidget(header, 0, 5)
		header = HeadingLabel()
		header.setTextAlign(QtCore.Qt.AlignHCenter)
		header.setText("ILimit")
		gridLayout.addWidget(header, 0, 6)
		
		# Roll coefficients
		label = QtGui.QLabel("Roll")
		gridLayout.addWidget(label, 1, 1)
		self.spbRollKp = QtGui.QDoubleSpinBox()
		self.spbRollKp.setDecimals(5)
		self.spbRollKp.setMinimum(0)
		self.spbRollKp.setMaximum(50)
		self.spbRollKp.setMinimumWidth(150)
		gridLayout.addWidget(self.spbRollKp, 1, 3)
		self.spbRollKi = QtGui.QDoubleSpinBox()
		self.spbRollKi.setDecimals(5)
		self.spbRollKi.setMinimum(0)
		self.spbRollKi.setMaximum(50)
		self.spbRollKi.setMinimumWidth(150)
		gridLayout.addWidget(self.spbRollKi, 1, 4)
		self.spbRollKd = QtGui.QDoubleSpinBox()
		self.spbRollKd.setDecimals(5)
		self.spbRollKd.setMinimum(0)
		self.spbRollKd.setMaximum(50)
		self.spbRollKd.setMinimumWidth(150)
		gridLayout.addWidget(self.spbRollKd, 1, 5)
		self.spbRollILimit = QtGui.QDoubleSpinBox()
		self.spbRollILimit.setDecimals(5)
		self.spbRollILimit.setMinimum(0)
		self.spbRollILimit.setMaximum(50)
		self.spbRollILimit.setMinimumWidth(150)
		gridLayout.addWidget(self.spbRollILimit, 1, 6)
		
		# Pitch coefficients
		label = QtGui.QLabel("Pitch")
		gridLayout.addWidget(label, 2, 1)
		self.spbPitchKp = QtGui.QDoubleSpinBox()
		self.spbPitchKp.setDecimals(5)
		self.spbPitchKp.setMinimum(0)
		self.spbPitchKp.setMaximum(50)
		self.spbPitchKp.setMinimumWidth(150)
		gridLayout.addWidget(self.spbPitchKp, 2, 3)
		self.spbPitchKi = QtGui.QDoubleSpinBox()
		self.spbPitchKi.setDecimals(5)
		self.spbPitchKi.setMinimum(0)
		self.spbPitchKi.setMaximum(50)
		self.spbPitchKi.setMinimumWidth(150)
		gridLayout.addWidget(self.spbPitchKi, 2, 4)
		self.spbPitchKd = QtGui.QDoubleSpinBox()
		self.spbPitchKd.setDecimals(5)
		self.spbPitchKd.setMinimum(0)
		self.spbPitchKd.setMaximum(50)
		self.spbPitchKd.setMinimumWidth(150)
		gridLayout.addWidget(self.spbPitchKd, 2, 5)
		self.spbILimitPitch = QtGui.QDoubleSpinBox()
		self.spbILimitPitch.setDecimals(5)
		self.spbILimitPitch.setMinimum(0)
		self.spbILimitPitch.setMaximum(50)
		self.spbILimitPitch.setMinimumWidth(150)
		gridLayout.addWidget(self.spbILimitPitch, 2, 6)

		# Yaw coefficients
		label = QtGui.QLabel("Yaw")
		gridLayout.addWidget(label, 3, 1)
		self.spbYawKp = QtGui.QDoubleSpinBox()
		self.spbYawKp.setDecimals(5)
		self.spbYawKp.setMinimum(0)
		self.spbYawKp.setMaximum(50)
		self.spbYawKp.setMinimumWidth(150)
		gridLayout.addWidget(self.spbYawKp, 3, 3)
		self.spbYawKi = QtGui.QDoubleSpinBox()
		self.spbYawKi.setDecimals(5)
		self.spbYawKi.setMinimum(0)
		self.spbYawKi.setMaximum(50)
		self.spbYawKi.setMinimumWidth(150)
		gridLayout.addWidget(self.spbYawKi, 3, 4)
		self.spbYawKd = QtGui.QDoubleSpinBox()
		self.spbYawKd.setDecimals(5)
		self.spbYawKd.setMinimum(0)
		self.spbYawKd.setMaximum(50)
		self.spbYawKd.setMinimumWidth(150)
		gridLayout.addWidget(self.spbYawKd, 3, 5)
		self.spbILimitYaw = QtGui.QDoubleSpinBox()
		self.spbILimitYaw.setDecimals(5)
		self.spbILimitYaw.setMinimum(0)
		self.spbILimitYaw.setMaximum(50)
		self.spbILimitYaw.setMinimumWidth(150)
		gridLayout.addWidget(self.spbILimitYaw, 3, 6)
		

		# DCM Constants
		groupBox = QtGui.QGroupBox("Attitude Stabilization (Outer Loop)")
		gridLayout = QtGui.QGridLayout()
		groupBox.setLayout(gridLayout)
		gridLayout.setSpacing(4)
		mainLayout.addWidget(groupBox)
		gridLayout.setColumnMinimumWidth(0, 20)
		gridLayout.setColumnMinimumWidth(1, 80)
		gridLayout.setColumnMinimumWidth(2, 20)
		gridLayout.setColumnMinimumWidth(3, 100)
		gridLayout.setColumnMinimumWidth(4, 100)
		gridLayout.setColumnMinimumWidth(5, 150)
		gridLayout.setColumnStretch(0, 0)
		gridLayout.setColumnStretch(1, 1)
		gridLayout.setColumnStretch(2, 0)
		gridLayout.setColumnStretch(3, 1)
		gridLayout.setColumnStretch(4, 1)
		gridLayout.setColumnStretch(5, 2)
		# Add spacer items
		spacer = QtGui.QSpacerItem(20, 20)
		gridLayout.addItem(spacer, 0, 0)
		spacer = QtGui.QSpacerItem(20, 20)
		gridLayout.addItem(spacer, 0, 2)
		spacer = QtGui.QSpacerItem(150, 20)
		gridLayout.addItem(spacer, 0, 5)
		spacer = QtGui.QSpacerItem(50, 120)
		gridLayout.addItem(spacer, 3, 0)      

		# Headers
		header = HeadingLabel()
		header.setTextAlign(QtCore.Qt.AlignHCenter)
		header.setText("Proportional")
		gridLayout.addWidget(header, 0, 3)
		header = HeadingLabel()
		header.setText("Integral")
		header.setTextAlign(QtCore.Qt.AlignHCenter)
		gridLayout.addWidget(header, 0, 4)

		# DCM Roll/Pitch
		label = QtGui.QLabel("Roll/Pitch")
		gridLayout.addWidget(label, 1, 1)
		self.spbDcmRollPitchPFactor = QtGui.QDoubleSpinBox()
		self.spbDcmRollPitchPFactor.setDecimals(5)
		self.spbDcmRollPitchPFactor.setMinimum(0)
		self.spbDcmRollPitchPFactor.setMaximum(50)
		self.spbDcmRollPitchPFactor.setMinimumWidth(150)
		gridLayout.addWidget(self.spbDcmRollPitchPFactor, 1, 3)
		self.spbDcmRollPitchIFactor = QtGui.QDoubleSpinBox()
		self.spbDcmRollPitchIFactor.setDecimals(5)
		self.spbDcmRollPitchIFactor.setMinimum(0)
		self.spbDcmRollPitchIFactor.setMaximum(50)
		self.spbDcmRollPitchIFactor.setMinimumWidth(150)
		gridLayout.addWidget(self.spbDcmRollPitchIFactor, 1, 4)

		# DCM Yaw
		label = QtGui.QLabel("Yaw")
		gridLayout.addWidget(label, 2, 1)
		self.spbDcmYawPFactor = QtGui.QDoubleSpinBox()
		self.spbDcmYawPFactor.setDecimals(5)
		self.spbDcmYawPFactor.setMinimum(0)
		self.spbDcmYawPFactor.setMaximum(50)
		self.spbDcmYawPFactor.setMinimumWidth(150)
		gridLayout.addWidget(self.spbDcmYawPFactor, 2, 3)
		self.spbDcmYawIFactor = QtGui.QDoubleSpinBox()
		self.spbDcmYawIFactor.setDecimals(5)
		self.spbDcmYawIFactor.setMinimum(0)
		self.spbDcmYawIFactor.setMaximum(50)
		self.spbDcmYawIFactor.setMinimumWidth(150)
		gridLayout.addWidget(self.spbDcmYawIFactor, 2, 4)

		# Stick range and limits
		groupBox = QtGui.QGroupBox("Stick ranges and limits")
		gridLayout = QtGui.QGridLayout()
		groupBox.setLayout(gridLayout)
		gridLayout.setSpacing(4)
		mainLayout.addWidget(groupBox)
		gridLayout.setColumnMinimumWidth(0, 20)
		gridLayout.setColumnMinimumWidth(1, 80)
		gridLayout.setColumnMinimumWidth(2, 20)
		gridLayout.setColumnMinimumWidth(3, 100)
		gridLayout.setColumnMinimumWidth(4, 100)
		gridLayout.setColumnMinimumWidth(5, 100)
		gridLayout.setColumnMinimumWidth(6, 150)
		gridLayout.setColumnStretch(0, 0)
		gridLayout.setColumnStretch(1, 1)
		gridLayout.setColumnStretch(2, 0)
		gridLayout.setColumnStretch(3, 1)
		gridLayout.setColumnStretch(4, 1)
		gridLayout.setColumnStretch(5, 1)
		gridLayout.setColumnStretch(6, 2)
		# Add spacer items
		spacer = QtGui.QSpacerItem(20, 20)
		gridLayout.addItem(spacer, 0, 0)
		spacer = QtGui.QSpacerItem(20, 20)
		gridLayout.addItem(spacer, 0, 2)
		spacer = QtGui.QSpacerItem(150, 20)
		gridLayout.addItem(spacer, 0, 6)
		spacer = QtGui.QSpacerItem(50, 120)
		gridLayout.addItem(spacer, 4, 0)      

		# Headers
		header = HeadingLabel()
		header.setTextAlign(QtCore.Qt.AlignHCenter)
		header.setText("Roll")
		gridLayout.addWidget(header, 0, 3)
		header = HeadingLabel()
		header.setTextAlign(QtCore.Qt.AlignHCenter)
		header.setText("Pitch")
		gridLayout.addWidget(header, 0, 4)
		header = HeadingLabel()
		header.setTextAlign(QtCore.Qt.AlignHCenter)
		header.setText("Yaw")
		gridLayout.addWidget(header, 0, 5)

		# Full stick angle
		label = QtGui.QLabel("Full Stick Angle (deg)")
		gridLayout.addWidget(label, 1, 1)
		self.spbStickAngleRoll = QtGui.QDoubleSpinBox()
		self.spbStickAngleRoll.setMinimumWidth(150)
		gridLayout.addWidget(self.spbStickAngleRoll, 1, 3)
		self.spbStickAnglePitch = QtGui.QDoubleSpinBox()
		self.spbStickAnglePitch.setMinimumWidth(150)
		gridLayout.addWidget(self.spbStickAnglePitch, 1, 4)
		self.spbStickAngleYaw = QtGui.QDoubleSpinBox()
		self.spbStickAngleYaw.setMinimumWidth(150)
		gridLayout.addWidget(self.spbStickAngleYaw, 1, 5)

		# Full stick rate
		label = QtGui.QLabel("Full Stick Rate (deg/s)")
		gridLayout.addWidget(label, 2, 1)
		self.spbStickRateRoll = QtGui.QDoubleSpinBox()
		self.spbStickRateRoll.setMinimumWidth(150)
		gridLayout.addWidget(self.spbStickRateRoll, 2, 3)
		self.spbStickRatePitch = QtGui.QDoubleSpinBox()
		self.spbStickRatePitch.setMinimumWidth(150)
		gridLayout.addWidget(self.spbStickRatePitch, 2, 4)
		self.spbStickRateYaw = QtGui.QDoubleSpinBox()
		self.spbStickRateYaw.setMinimumWidth(150)
		gridLayout.addWidget(self.spbStickRateYaw, 2, 5)

		# Maximum rate in attitude mode
		label = QtGui.QLabel("Maximum Rate in Attitude Mode (deg/s)")
		gridLayout.addWidget(label, 3, 1)
		self.spbStickMaxRateAttRoll = QtGui.QDoubleSpinBox()
		self.spbStickMaxRateAttRoll.setMinimumWidth(150)
		gridLayout.addWidget(self.spbStickMaxRateAttRoll, 3, 3)
		self.spbStickMaxRateAttPitch = QtGui.QDoubleSpinBox()
		self.spbStickMaxRateAttPitch.setMinimumWidth(150)
		gridLayout.addWidget(self.spbStickMaxRateAttPitch, 3, 4)
		self.spbStickMaxRateAttYaw = QtGui.QDoubleSpinBox()
		self.spbStickMaxRateAttYaw.setMinimumWidth(150)
		gridLayout.addWidget(self.spbStickMaxRateAttYaw, 3, 5)

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
												self.spbILimitPitch.value())

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