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


import os, sys
import importlib
from ..logger import Logger


OBJECT_MODULES_DEFINITION_FILE = "objectmodules.txt"


class VTOLObjectManager:
    """Manages all VTOL objects."""
    def __init__(self):
        self._objects = list()
        self._logger = Logger()

        # Register all objects
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
                    print vtolObject.OBJID
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
        """Removes the object with the specified id."""
        pass

    def getObjects(self):
        return self._objects

    def getObjectById(self, id):
        pass

    def getObjectByName(self, name):
        pass

    def unpack(self, id, data):
        pass

    def pack(self, id):
        pass

    def _objectExists(self, id):
        for object in self._objects:
            if object.objId == id:
                return True

        return False
