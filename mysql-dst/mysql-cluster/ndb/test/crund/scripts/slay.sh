#!/bin/bash

# Copyright (c) 2010, 2013, Oracle and/or its affiliates. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

#XXX fix: also kills this script, possibly before other matching processes
ps -efa | grep -i $1

for X in `ps -efa | grep -i $1 | awk {'print $2'}`; do
  kill $X;
done

sleep 1

for X in `ps -efa | grep -i $1 | awk {'print $2'}`; do
  kill -9 $X;
done
 