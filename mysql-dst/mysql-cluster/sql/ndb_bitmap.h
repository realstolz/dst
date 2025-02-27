/*
   Copyright (c) 2016, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

#ifndef NDB_BITMAP_H
#define NDB_BITMAP_H

#include <my_global.h>
#include <my_bitmap.h>


/**
  Initialize bitmap using provided buffer.
  @param bitmap     The MY_BITMAP to initialize
  @param buf        Buffer to hold the bits for the bitmap
  @param num_bits   Max number of bits to store in the bitmap

  NOTE! Since no memory need to be allocated the 'bitmap_init' funtion
  never fails.

  NOTE! Size of provided buffer automatically defferred by
  usage of template and thus it's possible to check that
  bitmap is not initialized larger than what the buffer can hold.

*/

template<size_t sz>
static inline
void ndb_bitmap_init(MY_BITMAP& bitmap,
                     my_bitmap_map (&buf)[sz],
                     uint num_bits)
{
  assert(num_bits > 0);
  assert(bitmap_buffer_size(num_bits) <= (sz * sizeof(my_bitmap_map)));

  // Function never fails when called with a "buf" provided
  (void)bitmap_init(&bitmap, buf, num_bits, false);
}

#endif
 