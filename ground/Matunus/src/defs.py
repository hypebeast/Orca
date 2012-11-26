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

app_defs = None

class _AppDefs:
	AppPath = ""
	DataPath = ""
	IconsPath = ""
	ArtworkPath = ""
	SrcPath = ""
	QmlPath = ""
	StylesheetPath = ""
	
	def __init__(self):
		pass

def AppDefs():
	global app_defs

	if not app_defs:
		app_defs = _AppDefs()
	return app_defs
