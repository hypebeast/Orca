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

import usb.core
import usb.util

class CommandMessage:
    def __init__(self, commandType, data):
        self.commandType = commandType
        self.data = data

    def setCommandType(self, commandType):
        self.commandType = commandType

    def setData(self, data):
        self.data = data

    def getMessage(self):
        return None


class UsbConnection:
    def __init__(self):
        self.vendorId = 0x0000
        self.productId = 0x0000
        self.connected = False

    def connect(self):
        pass

    def disconnect(self):
        pass

    def scan(self):
        pass

    def readStatus(self):
        pass
