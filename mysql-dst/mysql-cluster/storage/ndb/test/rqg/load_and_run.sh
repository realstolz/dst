#!/bin/sh

# Copyright (c) 2011, 2013, Oracle and/or its affiliates. All rights reserved.
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

set -e

base="`dirname $0`"
source "$base"/parseargs.sh

ecp="set optimizer_switch = 'engine_condition_pushdown=on';"

if [ "$load" ]
then
    source "$base"/load.sh
fi

if [ "$runtime" -o "$loops" ]
then
    source "$base"/run.sh
fi
 