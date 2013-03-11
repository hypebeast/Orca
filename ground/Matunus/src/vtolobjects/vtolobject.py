# Copyright (C) 2012-2013 Sebastian Ruml <sebastian.ruml@gmail.com>
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

from vtolobjectmetadata import VTOLObjectMetadata


class VTOLObject(object):
    """
    Base VTOL object implementation. Every VTOL object needs to derive this
    from this class.
    """

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

    def __init__(self, objId, name, description):
        self._metadata = VTOLObjectMetadata(objId)
        self._objId = objId
        self._name = name
        self._description = description
        self._instId = 0
        # Actual data
        self._fields = []

    def addField(self, field):
        """Adds a data field to this VTOL object."""
        self._fields.append(field)

    def getField(self, fieldName):
        """
        Returns the field for the given name. Otherwise, none if field was not
        found.
        """
        for field in self._fields:
            if fieldName == field.name:
                return field

        return None

    def setField(self, fieldName, value):
        """Sets a field with the given name to the given value."""
        for field in self._fields:
            if field.name == fieldName:
                field.value = value

    def getFields(self):
        """Return all data fields."""
        return self._fields

    def getName(self):
        return self._name

    def getDescription(self):
        return self._description

    def load(self, file):
        pass

    def save(self, file):
        pass

    def getMetadata(self):
        """"Gets the metadata."""
        return self._metadata

    def id():
        def fget(self):
            return self._objId

        def fset(self, value):
            self._objId = value
        return locals()
    id = property(**id())
