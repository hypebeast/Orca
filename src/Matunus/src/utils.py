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


###############################################################################
## Part of these functions are from
## http://ottawarobotics.org/subversion/Members/andrzej/PYTHON/PyDaq/src/PyDaq/Sandbox/Eli_Benderskys_demos/SerialMon/eblib
###############################################################################


import Queue
import re, itertools
import _winreg as winreg


def get_all_from_queue(Q):
    """ Generator to yield one after the others all items 
        currently in the queue Q, without any waiting.
    """
    try:
        while True:
            yield Q.get_nowait( )
    except Queue.Empty:
        raise StopIteration


def get_item_from_queue(Q, timeout=0.01):
    """ Attempts to retrieve an item from the queue Q. If Q is
        empty, None is returned.
        
        Blocks for 'timeout' seconds in case the queue is empty,
        so don't use this method for speedy retrieval of multiple
        items (use get_all_from_queue for that).
    """
    try: 
        item = Q.get(True, 0.01)
    except Queue.Empty: 
        return None
    
    return item


def full_port_name(portname):
    """ Given a port-name (of the form COM7, 
        COM12, CNCA0, etc.) returns a full 
        name suitable for opening with the 
        Serial class.
    """
    m = re.match('^COM(\d+)$', portname)
    if m and int(m.group(1)) < 10:
        return portname    
    return '\\\\.\\' + portname    
    

def enumerate_serial_ports():
    """ Uses the Win32 registry to return an 
        iterator of serial (COM) ports 
        existing on this computer.
    """
    path = 'HARDWARE\\DEVICEMAP\\SERIALCOMM'
    try:
        key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, path)
    except WindowsError:
        raise IterationError

    for i in itertools.count():
        try:
            val = winreg.EnumValue(key, i)
            yield str(val[1])
        except EnvironmentError:
            break