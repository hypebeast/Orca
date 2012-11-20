#!/usr/bin/env python

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

__author__ = 'Sebastian Ruml'


import sys
import os
from optparse import OptionParser
import ctypes

from src import MainAppWindow
from src import defs

__appName__ = 'Matunus'
__version__ = '0.2.0'
__author__ = 'Sebastian Ruml'

# Check that at least Python 2.6 is running
if sys.version_info < (2, 6, 0):
    print ('Python version must be at least 2.6.')
    sys.exit(1)

#if platform.system() != "Darwin":
    #print ('Mac OS X are supported!')
    #sys.exit(1)

def main():
    """
    Everything dispatches from this main function.
    """
    # Initialize all app definitions
    app_defs = defs.AppDefs()
    app_defs.AppPath = os.path.dirname(os.path.abspath(__file__))
    app_defs.DataPath = os.path.join(app_defs.AppPath, "data")
    app_defs.IconsPath = os.path.join(app_defs.DataPath, "icons")
    app_defs.ArtworkPath = os.path.join(app_defs.DataPath, "artwork")
    app_defs.SrcPath = os.path.join(app_defs.AppPath, "src")
    app_defs.QmlPath = os.path.join(app_defs.SrcPath, "qml")

    # Command line arguments
    parser = OptionParser()
    parser.add_option("-v", "--version", dest="version", action="store_true",
                        help="Prints the version and exists", default=False)
    parser.add_option("-d", "--debug", dest="debug", action="store_true",
                        help="Prints debug information to stdout", default=False)
    parser.add_option("-q", "--quit", dest="verbose", action="store_false",
                        help="Don't print status message to stdout", default=True)
    options, args = parser.parse_args()

    if options.version:
        print __appName__ + " v" + __version__
        sys.exit(2)

    # Set new AppUserModelID
    # TODO: Only required for Windows
    myappid = 'sr.orca.matunus.002' # arbitrary string
    ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID(myappid)

    # Start the application
    app = MainAppWindow.App(options, args)


if __name__ == '__main__':
    try:
        main()
    except (KeyboardInterrupt, SystemExit):
        sys.exit(1)
    except:
        import traceback
        traceback.print_exc()
