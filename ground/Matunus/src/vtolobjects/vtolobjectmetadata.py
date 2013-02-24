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


class VTOLObjectMetadata(object):
    class UpdateMode:
        PERIODIC = 0
        ONCHANGE = 1
        MANUAL = 2
        NEVER = 3

    class Access:
        READWRITE = 0
        READONLY = 1

    def __init__(self, objId):
        self.access = VTOLObjectMetadata.Access.READWRITE
        self.gcsAccess = VTOLObjectMetadata.Access.READWRITE
        self.telemetryAcked = False
        self.telemetryUpdateMode = VTOLObjectMetadata.UpdateMode.MANUAL
        self.telemetryUpdatePeriod = 0
        self.gcsTelemetryAcked = False
        self.gcsTelemetryUpdateMode = VTOLObjectMetadata.UpdateMode.MANUAL
        self.gcsTelemetryUpdatePeriod = 0
        self.loggingUpdateMode = 0
        self.loggingUpdatePeriod = VTOLObjectMetadata.UpdateMode.MANUAL
        self.objId = objId

    def read(self):
        pass

    def write(self):
        pass
