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

from vtolobjectmetadata import VTOLObjectMetadata
from vtolobjectfield import VTOLObjectField


class VTOLObject:
    """
    Base VTOL object implementation. Every VTOL object needs to derive this
    from this class.
    """
    def __init__(self, objId, name, description):
        self.metadata = VTOLObjectMetadata(objId)
        self.objId = objId
        self.name = name
        self.description = description
        self.instId = 0
        self.fields = []

    def addField(self, field):
        """Adds a data field to this VTOL object."""
        self.fields.append(field)

    def getField(self, fieldName):
        """
        Returns the field for the given name. Otherwise, none if field was not
        found.
        """
        for field in self.fields:
            if fieldName == field.name:
                return field

        return None

    def write(self):
        pass

    def read(self):
        pass

    def pack(self):
        """
        Packs this object to a byte array.
        """
        pass

    def unpack(self, data):
        """
        Unpacks this object from an byte array.
        """
        pass

    def load(self, file):
        pass

    def save(self, file):
        pass
