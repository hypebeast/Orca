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


import sys
import os
import struct

try:
    from PyQt4.QtCore import pyqtSignal, QObject
except ImportError:
    print "No PyQt found!"
    sys.exit(2)

from ..logger import Logger
from vtolobject import VTOLObject
from vtolobjectfield import VTOLObjectField


OBJECT_MODULES_DEFINITION_FILE = "objectmodules.txt"

# Global VTOLObjectManager object
vtolObjectManager = None


class _VTOLObjectManager:
    """Manages all VTOL objects."""

    class TypeSizes:
        """Type sizes in bytes"""
        ENUM = 4
        INT8 = 1
        INT16 = 2
        INT32 = 4
        UINT8 = 1
        UINT16 = 2
        UINT32 = 4
        FLOAT = 4

    def __init__(self):
        self._objects = list()
        self._logger = Logger()

        # Maps the field types to their format character
        self.formatCharacterTable = {
                            VTOLObjectField.FType.INT8: 'c',
                            VTOLObjectField.FType.INT16: 'h',
                            2: 'i',
                            3: 'B',
                            4: 'H',
                            5: 'I',
                            6: 'f',
                            7: 'i',
                        }
        self.typeSizesTable = {
                                0: 1,
                                1: 2,
                                2: 4,
                                3: 1,
                                4: 2,
                                5: 4,
                                6: 4,
                                7: 4
                            }

        # Register all VTOL objects
        self._registerObjects()

    def _registerObjects(self):
        """
        Registers all available VTOL objects.
        """
        self._logger.info("Register VTOL objects...")

        dir = os.path.dirname(os.path.realpath(__file__))
        with open(os.path.join(dir, OBJECT_MODULES_DEFINITION_FILE), 'r') as f:
            modules = f.readlines()
            modules = [mod.rstrip("\n") for mod in modules]
            for module in modules:
                classname = module
                self._logger.info("Register object: %s" % (module))
                mod = __import__(module, globals(), locals(), [classname], -1)
                try:
                    vtolObject = getattr(mod, classname)
                    self.addObject(vtolObject)
                except AttributeError as ex:
                    print ex.message
                    raise

    def addObject(self, object):
        """Add a new VTOL object."""
        if not object:
            return

        # Check if we have already an object with the same ID
        if self._objectExists(object.OBJID):
            self._logger.info("VTOL object \"%s\" already exists" % object.name)
            return

        # Add the object to the object list
        self._objects.append(object)

    def removeObject(self, id):
        """Removes the object with the specified id from the object list."""
        if not id:
            return

        if self._objectExists(id):
            self._objects.remove(self.getObjectById(id))

    def getObjects(self):
        """"Returns all available VTOL objects."""
        return self._objects

    def getObjectById(self, id):
        """Returns the object with the given ID. Otherwise, false."""
        if not id:
            return None

        for object in self._objects:
            if object.OBJID == id:
                return object

        return None

    def getObjectByName(self, name):
        """
        Returns the object with the given name. If the name isn't found it
        returns None.
        """
        if not name:
            return None

        for object in self._objects:
            if object.NAME == name:
                return object

        return None

    def unpack(self, id, data):
        """Unpacks the object with the given id from the given byte array."""
        object = self.getObjectById(id)

        if object == None:
            return

        dataFields = object.getFields()
        offset = 0
        for i in xrange(0, len(dataFields)):
            field = dataFields[i]
            name = field.name
            typeChar = self.formatCharacterTable[field.ftype]
            value = struct.unpack_from(typeChar, data, offset)[0]
            offset += self.typeSizesTable[field.ftype]
            object.setField(name, value)

    def pack(self, id):
        """Packs the object with the given id to an byte array."""
        object = self.getObjectById(id)

        if object == None:
            return None

        data = bytearray()
        dataFields = object.getFields()
        for i in xrange(0, len(dataFields)):
            field = dataFields[i]
            name = field.name
            typeChar = self.formatCharacterTable[field.ftype]
            data.extend(struct.pack(typeChar, object.getField(name)))

        return data

        return object.pack()

    def _objectExists(self, id):
        """Returns True if the object with the given ID exists. Otherwise, False."""
        for object in self._objects:
            if object.OBJID == id:
                return True

        return False


def VTOLObjectManager():
    global vtolObjectManager

    if vtolObjectManager == None:
        vtolObjectManager = _VTOLObjectManager()

    return vtolObjectManager
