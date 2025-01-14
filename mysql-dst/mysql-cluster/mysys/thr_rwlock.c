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
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

/* Synchronization - readers / writer thread locks */

#include "thr_rwlock.h"

int rw_pr_init(rw_pr_lock_t *rwlock)
{
  native_mutex_init(&rwlock->lock, NULL);
  native_cond_init(&rwlock->no_active_readers);
  rwlock->active_readers= 0;
  rwlock->writers_waiting_readers= 0;
  rwlock->active_writer= FALSE;
#ifdef SAFE_MUTEX
  rwlock->writer_thread= 0;
#endif
  return 0;
}


int rw_pr_destroy(rw_pr_lock_t *rwlock)
{
  native_cond_destroy(&rwlock->no_active_readers);
  native_mutex_destroy(&rwlock->lock);
  return 0;
}


int rw_pr_rdlock(rw_pr_lock_t *rwlock)
{
  native_mutex_lock(&rwlock->lock);
  /*
    The fact that we were able to acquire 'lock' mutex means
    that there are no active writers and we can acquire rd-lock.
    Increment active readers counter to prevent requests for
    wr-lock from succeeding and unlock mutex.
  */
  rwlock->active_readers++;
  native_mutex_unlock(&rwlock->lock);
  return 0;
}


int rw_pr_wrlock(rw_pr_lock_t *rwlock)
{
  native_mutex_lock(&rwlock->lock);

  if (rwlock->active_readers != 0)
  {
    /* There are active readers. We have to wait until they are gone. */
    rwlock->writers_waiting_readers++;

    while (rwlock->active_readers != 0)
      native_cond_wait(&rwlock->no_active_readers, &rwlock->lock);

    rwlock->writers_waiting_readers--;
  }

  /*
    We own 'lock' mutex so there is no active writers.
    Also there are no active readers.
    This means that we can grant wr-lock.
    Not releasing 'lock' mutex until unlock will block
    both requests for rd and wr-locks.
    Set 'active_writer' flag to simplify unlock.

    Thanks to the fact wr-lock/unlock in the absence of
    contention from readers is essentially mutex lock/unlock
    with a few simple checks make this rwlock implementation
    wr-lock optimized.
  */
  rwlock->active_writer= TRUE;
#ifdef SAFE_MUTEX
  rwlock->writer_thread= my_thread_self();
#endif
  return 0;
}


int rw_pr_unlock(rw_pr_lock_t *rwlock)
{
  if (rwlock->active_writer)
  {
    /* We are unlocking wr-lock. */
#ifdef SAFE_MUTEX
    rwlock->writer_thread= 0;
#endif
    rwlock->active_writer= FALSE;
    if (rwlock->writers_waiting_readers)
    {
      /*
        Avoid expensive cond signal in case when there is no contention
        or it is wr-only.

        Note that from view point of performance it would be better to
        signal on the condition variable after unlocking mutex (as it
        reduces number of contex switches).

        Unfortunately this would mean that such rwlock can't be safely
        used by MDL subsystem, which relies on the fact that it is OK
        to destroy rwlock once it is in unlocked state.
      */
      native_cond_signal(&rwlock->no_active_readers);
    }
    native_mutex_unlock(&rwlock->lock);
  }
  else
  {
    /* We are unlocking rd-lock. */
    native_mutex_lock(&rwlock->lock);
    rwlock->active_readers--;
    if (rwlock->active_readers == 0 &&
        rwlock->writers_waiting_readers)
    {
      /*
        If we are last reader and there are waiting
        writers wake them up.
      */
      native_cond_signal(&rwlock->no_active_readers);
    }
    native_mutex_unlock(&rwlock->lock);
  }
  return 0;
}


 