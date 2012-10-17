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
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)

import defs
from SvgDocument import SvgDocument


class UavStatusGadget(QtGui.QWidget):
    """
    SVG widget for displaying the basic UAV status.
    """
    def __init__(self):
        QtGui.QWidget.__init__(self)

        self.app_defs = defs.AppDefs()
        fileName = os.path.join(self.app_defs.ArtworkPath, "flight_controller_status.svg")

        self.svgDocument = SvgDocument(fileName)

        self.linkElement = self.svgDocument.elementById("linkStatus").toElement()

        self._initUI()

    def _initUI(self):
        self.setMinimumWidth(100)
        self.setMinimumHeight(100)

    def paintEvent(self, e):
        qp = QtGui.QPainter()
        qp.begin(self)
        self._drawWidget(qp)
        qp.end()

    def _drawWidget(self, qp):
        svgRenderer = QtSvg.QSvgRenderer()
        svgRenderer.load(self.svgDocument.document.toByteArray())
        svgRenderer.render(qp)

    def setLinkStatus(self, enabled=False):
        color = "#FF0000"
        if enabled:
            color = "#00ff00"
        else:
            color = "#FF0000"

        styleText = str(self.linkElement.attribute("style"))
        # Find and replace the 'fill' text
        startIndex = styleText.find("fill:")
        newStyleText = styleText.replace(styleText[startIndex:startIndex+12], "fill:"+color)

        self.linkElement.setAttribute(QtCore.QString("style"), QtCore.QString(newStyleText))

        # Repaint the widget
        self.repaint()