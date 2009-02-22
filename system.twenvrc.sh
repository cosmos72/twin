#!/bin/bash
# Copyright (C) 2000 by Massimiliano Ghilardi
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
#

#
# This file reads global and user profiles (like /etc/profile and ~/.profile)
# then outputs all environment variables to let twin server know them too.
#

#
# Read the default system settings.
#
test -r /etc/profile     && { . /etc/profile     > /dev/null 2>&1 ; }
test -r ${HOME}/.bashrc  && { . ${HOME}/.bashrc  > /dev/null 2>&1 ; }
test -r ${HOME}/.profile && { . ${HOME}/.profile > /dev/null 2>&1 ; }
#

exec 1>&2

printenv || env || set

