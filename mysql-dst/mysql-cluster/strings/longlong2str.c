/* Copyright (c) 2000, 2001, 2004, 2006, 2007 MySQL AB, 2009 Sun Microsystems, Inc.
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
  Defines: longlong2str();

  longlong2str(dst, radix, val)
  converts the (longlong) integer "val" to character form and moves it to
  the destination string "dst" followed by a terminating NUL.  The
  result is normally a pointer to this NUL character, but if the radix
  is dud the result will be NullS and nothing will be changed.

  If radix is -2..-36, val is taken to be SIGNED.
  If radix is  2.. 36, val is taken to be UNSIGNED.
  That is, val is signed if and only if radix is.  You will normally
  use radix -10 only through itoa and ltoa, for radix 2, 8, or 16
  unsigned is what you generally want.

  _dig_vec is public just in case someone has a use for it.
  The definitions of itoa and ltoa are actually macros in m_string.h,
  but this is where the code is.

  Note: The standard itoa() returns a pointer to the argument, when int2str
	returns the pointer to the end-null.
	itoa assumes that 10 -base numbers are allways signed and other arn't.
*/

#include <my_global.h>
#include "m_string.h"

#ifndef ll2str

/*
  This assumes that longlong multiplication is faster than longlong division.
*/

char *ll2str(longlong val,char *dst,int radix, int upcase)
{
  char buffer[65];
  char *p;
  long long_val;
  char *dig_vec= upcase ? _dig_vec_upper : _dig_vec_lower;
  ulonglong uval= (ulonglong) val;

  if (radix < 0)
  {
    if (radix < -36 || radix > -2) return (char*) 0;
    if (val < 0) {
      *dst++ = '-';
      /* Avoid integer overflow in (-val) for LLONG_MIN (BUG#31799). */
      uval = (ulonglong)0 - uval;
    }
    radix = -radix;
  }
  else
  {
    if (radix > 36 || radix < 2) return (char*) 0;
  }
  if (uval == 0)
  {
    *dst++='0';
    *dst='\0';
    return dst;
  }
  p = &buffer[sizeof(buffer)-1];
  *p = '\0';

  while (uval > (ulonglong) LONG_MAX)
  {
    ulonglong quo= uval/(uint) radix;
    uint rem= (uint) (uval- quo* (uint) radix);
    *--p= dig_vec[rem];
    uval= quo;
  }
  long_val= (long) uval;
  while (long_val != 0)
  {
    long quo= long_val/radix;
    *--p= dig_vec[(uchar) (long_val - quo*radix)];
    long_val= quo;
  }
  while ((*dst++ = *p++) != 0) ;
  return dst-1;
}
#endif

#ifndef longlong10_to_str
char *longlong10_to_str(longlong val,char *dst,int radix)
{
  char buffer[65];
  char *p;
  long long_val;
  ulonglong uval= (ulonglong) val;

  if (radix < 0)
  {
    if (val < 0)
    {
      *dst++ = '-';
      /* Avoid integer overflow in (-val) for LLONG_MIN (BUG#31799). */
      uval = (ulonglong)0 - uval;
    }
  }

  if (uval == 0)
  {
    *dst++='0';
    *dst='\0';
    return dst;
  }
  p = &buffer[sizeof(buffer)-1];
  *p = '\0';

  while (uval > (ulonglong) LONG_MAX)
  {
    ulonglong quo= uval/(uint) 10;
    uint rem= (uint) (uval- quo* (uint) 10);
    *--p = _dig_vec_upper[rem];
    uval= quo;
  }
  long_val= (long) uval;
  while (long_val != 0)
  {
    long quo= long_val/10;
    *--p = _dig_vec_upper[(uchar) (long_val - quo*10)];
    long_val= quo;
  }
  while ((*dst++ = *p++) != 0) ;
  return dst-1;
}
#endif
 