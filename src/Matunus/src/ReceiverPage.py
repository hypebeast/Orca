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

from ReceiverStatusPage import ReceiverStatusPage
from FlightModeSettingsPage import FlightModeSettingsPage
from RcInputPage import RcInputPage


class ReceiverPage(QtGui.QWidget):
    def __init__(self):
        super(ReceiverPage, self).__init__()

        self.createUI()

    def createUI(self):
    	mainLayout = QtGui.QVBoxLayout()
    	mainLayout.setMargin(0)
    	#mainLayout.addSpacing(5)

        tabWidget = QtGui.QTabWidget()
        tabWidget.setTabPosition(QtGui.QTabWidget.North)
        tabWidget.setTabShape(QtGui.QTabWidget.Rounded)
        mainLayout.addWidget(tabWidget)

        page = ReceiverStatusPage()
        tabWidget.addTab(page, "Status")

        page = RcInputPage(self)
        tabWidget.addTab(page, "RC Input")

        page = FlightModeSettingsPage()
        tabWidget.addTab(page, "Flight Mode Settings")

    	self.setLayout(mainLayout)

    def update(self, data):
        pass