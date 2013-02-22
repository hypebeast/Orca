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


class VTOLLinkRxState:
    SYNC = 0
    TYPE = 1
    LENGTH = 2
    ID = 3
    INSTID = 4
    DATA = 5
    CRC = 6
    COMPLETE = 7
    ERROR = 8


class VTOLLinkMessageType:
    OBJECT = 0x20
    OBJECT_REQ = 0x21
    OBJECT_ACK = 0x22
    ACK = 0x23


class VTOLMessageData(object):
    def __init__(self):
        pass


class VTOLLink(object):
    def __init__(self):
        pass
