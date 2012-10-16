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


try:
 	from PyQt4 import QtCore, QtXml
	from PyQt4.Qt import Qt
except ImportError:
 	print "No PyQt found!"
	import sys
	sys.exit(2)

import defs


class SvgDocument:
 	"""
	Helper class for working with SVG XML documents.
	"""
	def __init__(self, fileName):
		if fileName == None:
			raise Exception("A SVG file must be specified")

		self.sourceSvgFileName = fileName
		self.document = QtXml.QDomDocument()

		# Initial read of the XML file
		self._parseXmlFile()

	def _parseXmlFile(self):
		svgFile = QtCore.QFile(self.sourceSvgFileName)

		if not self.document.setContent(svgFile):
			svgFile.close()
			raise Exception

		svgFile.close()

	def elementById(self, attributeValue):
		return self.elementByUniqueAttributeValue("id", attributeValue)

	def elementByUniqueAttributeValue(self, attributeName, attributeValue):
		docElem = self.document.documentElement()
		firstNode = docElem.firstChild()

		node = self._findElementByName(attributeName, attributeValue, firstNode)
		return node

	def _findElementByName(self, attributeName, attributeValue, node):
		# Try to convert the node to an element
		element = node.toElement()
		value = element.attribute(attributeName, "Element has no attribute with that name.")

		# We found our node. Stop recursion and return it.
		if attributeValue == value:
			return node

		if not node.firstChild().isNull():
			result = self._findElementByName(attributeName, attributeValue, node.firstChild())
			# If we found it; return it
			if not result.isNull():
				return result

		if not node.nextSibling().isNull():
			result = self._findElementByName(attributeName, attributeValue, node.nextSibling())

			# If we found it; return it
			if not result.isNull():
				return result

		# Matching node not found; return a null node
		return QtXml.QDomNode()