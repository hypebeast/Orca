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
    from PyQt4 import QtGui, QtCore
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)


class HeadingLabel(QtGui.QFrame):
    """
    This class implements a heading label for use in HMI applications.
    """
    def __init__(self):
        super(HeadingLabel, self).__init__()
        palette = QtGui.QPalette()
        palette.setColor(QtGui.QPalette.Background, QtGui.QColor(51, 51, 51))
        palette.setColor(QtGui.QPalette.WindowText, QtGui.QColor(255, 0, 0))

        self.setAutoFillBackground(True)
        self.setPalette(palette)
        self.setMinimumHeight(10)
        self.setFrameStyle(QtGui.QFrame.StyledPanel | QtGui.QFrame.Raised)
        self.setLineWidth(2)

        self.layout = QtGui.QHBoxLayout()
        self.layout.setMargin(1)
        self.setLayout(self.layout)
        self.label = QtGui.QLabel("")
        self.label.setStyleSheet("QLabel { font: bold; color: #FFFFFF; }")
        self.layout.addWidget(self.label)

        self.setObjectName("HeadingLabel")

        self.text = ""

    def setBackgroundColor(self, color):
        pass

    def setTextColor(self, color):
        pass

    def setText(self, text):
        self.text = text
        self.label.setText(self.text)

    def getText(self):
        return self.text

    def setSize(self, width, height):
        self.setGeometry(0, 0, width, height)

    def setTextAlign(self, alignment):
        self.layout.setAlignment(alignment)

   	def paintEvent(self, e):
		qp = QtGui.QPainter()
		qp.begin(self)
		self._drawWidget(qp)
		qp.end()

    def _drawWidget(self, qp):
    	pass
