# Copyright (C) 2012 Sebastian Ruml <sebastian.ruml@gmail.com>
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
#import sip
#sip.setapi('QVariant', 2)

from PyQt4 import QtGui

#try:
#    from PyQt4 import QtGui
#except ImportError:
#    print "No PyQt found!"
#    import sys
#    sys.exit(2)


class SettingsDialog(QtGui.QDialog):
	"""
	Settings dialog class.
	"""
	def __init__(self):
		super(SettingsDialog, self).__init__()

		self.create_ui()

	def create_ui(self):
		self.resize(800, 600)
		self.setWindowTitle("Settings")