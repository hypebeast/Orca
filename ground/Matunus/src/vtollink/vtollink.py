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

import struct

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
        self.rxBytes = 0
        self.txBytes = 0
        self.rxObjectBytes = 0
        self.txObjectBytes = 0
        self.rxObjects = 0
        self.txObjects = 0
        self.rxErrors = 0
        self.txErrors = 0


class VTOLMessageData(object):
    def __init__(self):
        self.rxPacketLength = 0
        self.txPacketLength = 0
        self.rxBytes = []
        self.rxState = VTOLLinkRxState.SYNC
        self.type = 0
        self.rxCount = 0
        self.rxPacketSize = 0
        self.objId = -1
        self.obj = None
        self.objLength = 0
        self.stats = VTOLLinkStats()


class VTOLLink(object):
    """
    Implements the VTOL Link protocol. It handles the de- and encoding from VTOL
    messages.
    """
    MAX_PAYLOAD_LENGTH = 256
    MAX_MESSAGE_LENGTH = 1 + 1 + 2 + 2 + 1 + MAX_PAYLOAD_LENGTH + 1
    HEADER_LENGTH = 7
    START_BYTE = 0x8D
    CHECKSUM_LENGTH = 1

    def __init__(self):
        self._messageData = VTOLMessageData()
        self._objectManager = vtolobjectmanager.VTOLObjectManager()

    def sendObject(self, obj, callback, instId=0, req=False, ackReq=False, timeoutMs=500):
        """Sends an VTOL object."""
        if ackReq:
            self._sendObject(obj, callback, instId, VTOLLinkMessageType.OBJECT_ACK, timeoutMs)
        elif req:
            self._sendObject(obj, callback, instId, VTOLLinkMessageType.OBJECT_REQ, timeoutMs)
        else:
            self._sendObject(obj, callback, instId, VTOLLinkMessageType.OBJECT, timeoutMs)

    def processInputStream(self, rxByte):
        """"Processes the received byte stream."""
        state = self._processInputByte(rxByte)

        if state == VTOLLinkRxState.COMPLETE:
            self._receiveObject()

    def _processInputByte(self, rxByte):
        """"Processes an received byte."""
        if not rxByte:
            return

        if (self._messageData.rxState == VTOLLinkRxState.ERROR or
                self._messageData.rxState == VTOLLinkRxState.COMPLETE):
            self._messageData.rxState = VTOLLinkRxState.SYNC

        if self._messageData.rxState == VTOLLinkRxState.SYNC:
            if rxByte == self.START_BYTE:
                self._messageData.rxPacketLength = 1
                self._messageData.rxState = VTOLLinkRxState.TYPE
        elif self._messageData.rxState == VTOLLinkRxState.TYPE:
            if (rxByte != VTOLLinkMessageType.OBJECT or
                    rxByte != VTOLLinkMessageType.OBJECT_ACK or
                    rxByte != VTOLLinkMessageType.OBJECT_REQ or
                    rxByte != VTOLLinkMessageType.ACK):
                self._messageData.rxState = VTOLLinkRxState.ERROR

            self._messageData.type = rxByte
            self._messageData.rxPacketSize = 0
            self._messageData.rxCount = 0
            self._messageData.rxState = VTOLLinkRxState.LENGTH
        elif self._messageData.rxState == VTOLLinkRxState.LENGTH:
            if self._messageData.rxCount == 0:
                self._messageData.rxCount += 1
                self._messageData.rxPacketSize = rxByte

            self._messageData.rxCount = 0
            self._messageData.rxPacketSize += (rxByte << 8)
            self._messageData.objId = 0
            self._messageData.rxState = VTOLLinkRxState.ID
        elif self._messageData.rxState == VTOLLinkRxState.ID:
            if self._messageData.rxCount == 0:
                self._messageData.rxCount += 1
                self._messageData.objId = rxByte

            self._messageData.objId += (rxByte << 8)
            self._messageData.rxCount = 0

            # Search for object
            self._messageData.obj = self._objectManager.getObjectById(self._messageData.objId)

            # Get VTOLO size
            if self._messageData.obj:
                self._messageData.objLength = self._objectManager.getObjectSize(self._messageData.obj)

                # Check length
                if self._messageData.objLength <= self.MAX_PAYLOAD_LENGTH:
                    # Check if lengths match
                    if self._messageData.objLength + self.HEADER_LENGTH != self._messageData.rxPacketSize:
                        self._messageData.stats.rxErrors += 1
                        self._messageData.state = VTOLLinkRxState.ERROR
                    else:
                        self._messageData.rxState = VTOLLinkRxState.INSTID
                else:
                    self._messageData.stats.rxErrors += 1
                    self._messageData.rxState = VTOLLinkRxState.ERROR
            else:
                self._messageData.state = VTOLLinkRxState.ERROR
        elif self._messageData.rxState == VTOLLinkRxState.INSTID:
            # The instance ID is currently not supported
            self._messageData.rxState = VTOLLinkRxState.DATA
        elif self._messageData.rxState == VTOLLinkRxState.DATA:
            self._messageData.rxBytes.append(rxByte)
            self._messageData.stats.rxObjectBytes += 1
            self._messageData.rxCount += 1

            if self._messageData.rxCount >= self._messageData.objLength:
                self._messageData.rxState = VTOLLinkRxState.CRC
                self._messageData.rxCount = 0
        elif self._messageData.rxState == VTOLLinkRxState.CRC:
            # TODO: Check CRC
            self._messageData.stats.rxObjects += 1
            self._messageData.rxState = VTOLLinkRxState.COMPLETE
            self._messageData.rxCount = 0
        else:
            self._messageData.stats.rxErrors += 1
            self._messageData.rxState = VTOLLinkRxState.ERROR

    def _receiveObject(self):
        """"Processes an received VTOL object."""
        # Get VTOL object
        self._objectManager.getObjectById(self._messageData.objId)

        type = self._messageData.type
        if type == VTOLLinkMessageType.OBJECT:
            self._objectManager.unpack(self._messageData.objId, self._messageData.rxBytes)
        elif type == VTOLLinkMessageType.OBJECT_ACK:
            self._objectManager.unpack(self._messageData.objId, self._messageData.rxBytes)
            #self.sendObject()
        elif type == VTOLLinkMessageType.OBJECT_REQ:
            pass

    def _sendObject(self, obj, callback, instId, type, timeoutMs):
        """Sends an VTOL object."""
        self._sendSingleObject(obj, callback, type)

    def _sendSingleObject(self, obj, callback, type):
        """Sends an single VTOL object."""
        if not callback or not obj or not type:
            return

        if (type != VTOLLinkMessageType.OBJECT or
                type != VTOLLinkMessageType.OBJECT_ACK or
                type != VTOLLinkMessageType.OBJECT_REQ or
                type != VTOLLinkMessageType.ACK):
            return

        data = bytearray()

        # Add the start byte
        data.extend(struct.pack('B', self.START_BYTE))
        # Add the message type
        data.extend(struct.pack('B', type))
        # Add the payload length
        # TODO

        # Add the object ID
        data.extend(struct.pack('H', obj.id))
