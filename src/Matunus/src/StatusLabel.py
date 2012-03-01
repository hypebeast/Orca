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


# This is only needed for Python v2 but is harmless for Python v3.
import sip
sip.setapi('QVariant', 2)

try:
    from PyQt4 import QtGui
    from PyQt4 import QtCore
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)


class StatusLabel(QtGui.QFrame):
    def __init__(self):
        super(StatusLabel, self).__init__()
        palette = QtGui.QPalette()
        palette.setColor(QtGui.QPalette.Background, QtGui.QColor(71, 71, 71))

        self.setAutoFillBackground(True)
        self.setPalette(palette)
        self.setMinimumHeight(20)
        self.setMaximumHeight(50)
        self.setMinimumWidth(50)
        self.setGeometry(0, 0, 200, 20)
        self.setFrameStyle(QtGui.QFrame.Panel | QtGui.QFrame.Sunken)
        self.setLineWidth(3)

        layout = QtGui.QHBoxLayout()
        layout.setMargin(6)
        self.setLayout(layout)
        self.label = QtGui.QLabel("")
        self.label.setStyleSheet("QLabel { font: bold; background-color: #474747; color: #40FF1F; }")
        layout.addWidget(self.label)

        self.text = ""

    def setText(self, text):
        self.text = text
        self.label.setText(self.text)

    def getText(self):
        return self.text

    def sizeHint(self):
        return QtCore.QSize(200, 25)

    def setSize(self, width, height):
        self.setGeometry(0, 0, width, height)
