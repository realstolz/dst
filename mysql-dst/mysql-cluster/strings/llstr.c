/* Copyright (c) 2000, 2001, 2004, 2006, 2007 MySQL AB
   Use is subject to license terms.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

/*
  Defines: llstr();

  llstr(value, buff);

  This function saves a longlong value in a buffer and returns the pointer to
  the buffer.  This is useful when trying to portable print longlong
  variables with printf() as there is no usable printf() standard one can use.
*/


#include <my_global.h>
#include "m_string.h"

char *llstr(longlong value,char *buff)
{
  longlong10_to_str(value,buff,-10);
  return buff;
}

char *ullstr(longlong value,char *buff)
{
  longlong10_to_str(value,buff,10);
  return buff;
}
 