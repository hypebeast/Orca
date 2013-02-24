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

from ..vtolobjects import vtolobjectmanager


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


class VTOLLinkStats:
    def __init__(self):
        pass


class VTOLMessageData(object):
    def __init__(self):
        self.rxPacketLength = 0
        self.txPacketLength = 0
        self.rxBytes = []
        self.txBytes = []
        self.rxState = VTOLLinkRxState.SYNC
        self.type = 0
        self.rxCount = 0
        self.rxPacketSize = 0
        self.objId = -1
        self.obj = None
        self.objLength = 0


class VTOLLink(object):
    MAX_PAYLOAD_LENGTH = 256
    MAX_MESSAGE_LENGTH = 1 + 1 + 2 + 2 + 1 + MAX_PAYLOAD_LENGTH + 1
    HEADER_LENGTH = 7
    START_BYTE = 0x8D
    CHECKSUM_LENGTH = 1

    def __init__(self):
        self._messageData = VTOLMessageData()
        self._objectManager = vtolobjectmanager.VTOLObjectManager()

    def sendObject(self, obj, instId=0, ackReq=False, timeoutMs=500):
        """Sends an VTOL object"""
        pass

    def processInputStream(self, rxByte):
        """"Processes the received byte stream."""
        pass

    def _processInputByte(self, rxByte):
        """"Processes an received byte."""
        pass

    def _receiveObject(self, type, objId, instId, data, length):
        """"Processes an received object."""
        pass

    def _sendObject(self):
        pass

    def _sendSingleObject(self):
        pass
