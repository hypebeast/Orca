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
    from PyQt4 import QtGui, QtCore
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)

try:
	import PyQt4.Qwt5 as Qwt 
except:
	print "No PyQwt found!"
	import sys
	sys.exit(2)


class ScopeWidget(Qwt.QwtPlot):
	"""
	Widget that contains one scope.
	"""
	def __init__(self, dataFields, boardController):
		Qwt.QwtPlot.__init__(self)

		if dataFields is None or boardController is None:
			raise "Error!"

		self.boardController = boardController
		self.boardController.board_status_updated.connect(self._on_status_updated)
		self.plotCurves = list()
		self.updateTimer = QtCore.QTimer()
		QtCore.QObject.connect(self.updateTimer, QtCore.SIGNAL('timeout()'), self._on_timer)
		self.updateInterval = 0.5

		self.has_new_data = False

	def start(self):
		self.updateTimer.start(self.updateInterval * 1000.0)

	def stop(self):
		pass

	def _on_timer(self):
		self._update_scope()

	def _update_scope(self):
		pass

	def _on_status_updated(self):
		pass