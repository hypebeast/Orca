# Copyright (C) 2012 Sebastian Ruml <sebastian.ruml@gmail.com>
#
# This file is part of the Matunus project (part of the Orcacopter project)
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

import os

try:
    from PyQt4 import QtGui, QtCore, QtSvg
    from PyQt4.Qt import Qt
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)

import defs


class BoardStatusGadget(QtGui.QWidget):
	def __init__(self):
		super(BoardStatusGadget, self).__init__()

		self._initUI()

    def _initUI(self):
    	pass