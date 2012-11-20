__author__ = 'Sebastian Ruml'

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


import os

try:
    from PyQt4 import QtGui, QtSvg
    #from PyQt4.QtCore import QUrl
except ImportError:
    print "No PyQt found!"
    import sys
    sys.exit(2)

import defs

class WelcomePage(QtGui.QWidget):
    def __init__(self):
        super(WelcomePage, self).__init__()

        self.app_defs = defs.AppDefs()
        self._createUi()

    def _createUi(self):
        #self.view = QtGui.QGraphicsView()
        #self.scene = QtGui.QGraphicsScene()
        #self.scene.setSceneRect(self.rect())
        #self.view.setScene(self.scene)

        mainLayout = QtGui.QVBoxLayout()
        #mainLayout.addWidget(self.view)

        #image_file = os.path.join(self.app_defs.ArtworkPath, "welcome_background.png")
        #self.backgroundImage = QtGui.QGraphicsPixmapItem(QtGui.QPixmap(image_file), None, self.scene)

        label = QtGui.QLabel("<b><font size=\"8\" color='black'>Matunus</font></b>")
        mainLayout.addWidget(label)
        label = QtGui.QLabel("<font size=\"5\" color='black'>Orca Ground Control Station</font>")
        mainLayout.addWidget(label)

        #self.background = os.path.join(self.app_defs.ArtworkPath, "welcome_background.svg")

        # QML stuff
        #self.mainViewFile = os.path.join(self.app_defs.QmlPath, "welcomepage", "welcomepage.qml")
        #self.mainView = QtDeclarative.QDeclarativeView()
        #self.mainView.setSource(QUrl(self.mainViewFile))
        #self.mainView.setResizeMode(QtDeclarative.QDeclarativeView.SizeRootObjectToView)
        #mainLayout.addWidget(self.mainView)

        mainLayout.addStretch()
        self.setLayout(mainLayout)

    #def paintEvent(self, e):
    #    qp = QtGui.QPainter()
    #    qp.begin(self)
    #    #self._drawWidget(qp)
    #    qp.end()

    #def resizeEvent(self, event):
    #    self.scene.setSceneRect(0, 0, event.size().width(), event.size().height())
    #    super(WelcomePage, self).resizeEvent(event)


    def _drawWidget(self, qp):
        svgRenderer = QtSvg.QSvgRenderer()
        svgRenderer.load(self.background)
        svgRenderer.render(qp)

    def setSystemStatus(self, text):
        pass
        #self.systemStatus.setText(text)

    def setSystemMessage(self, message):
        pass
        #self.systemMessage.setText(message)
