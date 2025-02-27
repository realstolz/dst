/* Copyright (c) 2017, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA */

#include "my_dir.h"

#ifdef  __cplusplus
extern "C" {
#endif

/**
  Check if a file/dir is world-writable (only on non-Windows platforms)

  @param [in] Path of the file/dir to be checked

  @returns Status of the file/dir check
    @retval -2 Permission denied to check attributes of file/dir
    @retval -1 Error in reading file/dir
    @retval  0 File/dir is not world-writable
    @retval  1 File/dir is world-writable
 */

int is_file_or_dir_world_writable(const char *path)
{
  MY_STAT stat_info;
  (void)path; // avoid unused param warning when built on Windows
#ifndef _WIN32
  if (!my_stat(path, &stat_info, MYF(0)))
  {
      return (errno == EACCES) ? -2 : -1;
  }
  if ((stat_info.st_mode & S_IWOTH) &&
      ((stat_info.st_mode & S_IFMT) == S_IFREG ||   /* file   */
       (stat_info.st_mode & S_IFMT) == S_IFDIR))    /* or dir */
    return 1;
#endif
  return 0;
}

#ifdef  __cplusplus
}
#endif
 