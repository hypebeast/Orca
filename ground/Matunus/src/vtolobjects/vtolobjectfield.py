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


class VTOLObjectField:
    class FType:
        INT8 = 0
        INT16 = 1
        INT32 = 2
        UINT8 = 3
        UINT16 = 4
        UINT32 = 5
        FLOAT32 = 6
        ENUM = 7

    def __init__(self, name, ftype, units="", numElements=1):
        self.name = name
        self.ftype = ftype
        self.units = units
        self.numElements = numElements
        if ftype == VTOLObjectField.FType.FLOAT32:
            if numElements == 1:
                self.value = 0.0
            else:
                self.value = []
                for n in range(0, numElements):
                    self.value.append(0.0)
        else:
            if numElements == 1:
                self.value = 0
            else:
                self.value = []
                for n in range(0, numElements):
                    self.value.append(0)
