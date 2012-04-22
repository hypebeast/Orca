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

import sys
from optparse import OptionParser

from src import MainAppWindow

__appName__ = 'Matunus'
__version__ = '0.1.0'
__author__ = 'Sebastian Ruml'

# Check that at least Python 2.7 is running
if sys.version_info < (2, 7):
    print ('Python version must be at least 2.7.')
    sys.exit(1)

#if platform.system() != "Darwin":
    #print ('Mac OS X are supported!')
    #sys.exit(1)

# find out if they are asking for help
HELP = False
for val in sys.argv:
    if val == '-h' or val == '--help': HELP = True

def main():
    """
    Everything dispatches from this main function.
    """
    usage = "usage: %prog"

    app = MainAppWindow.App()


if __name__ == '__main__':
    try:
        main()
    except (KeyboardInterrupt, SystemExit):
        sys.exit(1)
    except:
        import traceback
        traceback.print_exc()
