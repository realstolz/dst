/* Copyright (c) 2000, 2015, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA */

/* Delete last read record */

#include "myrg_def.h"

int myrg_delete(MYRG_INFO *info, const uchar *record)
{
  if (!info->current_table)
  {
    set_my_errno(HA_ERR_NO_ACTIVE_RECORD);
    return HA_ERR_NO_ACTIVE_RECORD;
  }

  return mi_delete(info->current_table->table,record);
}
 