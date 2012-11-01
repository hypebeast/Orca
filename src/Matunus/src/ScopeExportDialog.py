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
    from PyQt4 import QtGui
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)


class ScopeExportDialog(QtGui.QDialog):
	def __init__(self, parent):
		if parent is None:
			raise Exception

		super(ScopeExportDialog, self).__init__(parent)

		self.selectedScope = 1
		self.fileName = ""
		self._createUI()

	def _createUI(self):
		self.setModal(True)
		self.setMinimumSize(300, 200)
		mainLayout = QtGui.QVBoxLayout()

		groupBox = QtGui.QGroupBox("Export Scope")
		mainLayout.addWidget(groupBox)
		vLayout = QtGui.QVBoxLayout()
		self.bScope1 = QtGui.QRadioButton("Scope 1")
		self.bScope1.setChecked(True)
		self.bScope1.clicked.connect(self._bScope1Clicked)
		self.bScope2 = QtGui.QRadioButton("Scope 2")
		self.bScope2.clicked.connect(self._bScope2Clicked)
		vLayout.addWidget(self.bScope1)
		vLayout.addWidget(self.bScope2)
		vLayout.addStretch()
		groupBox.setLayout(vLayout)

		mainLayout.addStretch()

		hLayout = QtGui.QHBoxLayout()
		self.bOk = QtGui.QPushButton("Ok")
		self.bOk.clicked.connect(self._okClicked)
		self.bCancel = QtGui.QPushButton("Cancel")
		self.bCancel.clicked.connect(self._cancelClicked)
		hLayout.addStretch()
		hLayout.addWidget(self.bOk)
		hLayout.addWidget(self.bCancel)
		mainLayout.addLayout(hLayout)

		self.setLayout(mainLayout)

	def _okClicked(self):
		# Show save file dialog
		fname = QtGui.QFileDialog.getSaveFileName(self, 'Export to...', 'c:', 'CSV (*.csv)')
		self.fileName = fname
		self.accept()

	def _cancelClicked(self):
		self.reject()

	def _bScope1Clicked(self):
		if self.bScope1.isChecked():
			self.selectedScope = 1

	def _bScope2Clicked(self):
		if self.bScope2.isChecked():
			self.selectedScope = 2