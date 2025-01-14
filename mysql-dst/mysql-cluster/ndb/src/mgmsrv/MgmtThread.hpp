/* Copyright (C) 2008 Sun Microsystems, Inc.
    All rights reserved. Use is subject to license terms.

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

#ifndef MgmtThread_H
#define MgmtThread_H

#include <ndb_global.h>
#include <NdbThread.h>

class MgmtThread {
  bool m_running;
  const char* m_name;
  size_t m_stack_size;
  NDB_THREAD_PRIO m_thread_prio;
  struct NdbThread* m_thread;

  static void* run_C(void* t) {
    MgmtThread *thread = (MgmtThread*)t;
    thread->run();
    return 0;
  }
public:
  MgmtThread(); // Not implemented
  MgmtThread(const MgmtThread&); // Not implemented
  MgmtThread(const char* name,
             size_t stack_size= 0, // Use default stack size
             NDB_THREAD_PRIO thread_prio= NDB_THREAD_PRIO_LOW) :
    m_running(true),
    m_name(name),
    m_stack_size(stack_size),
    m_thread_prio(thread_prio),
    m_thread(NULL){
  };
  virtual ~MgmtThread() {
    if (m_thread)
      stop();
  };

  virtual void run()= 0;
  bool start(){
    assert(m_running);
    m_thread = NdbThread_Create(run_C, (void**)this, m_stack_size,
                                m_name, m_thread_prio);
    return (m_thread != NULL);
  }
  bool stop(){
    void* res = 0;
    if (!m_thread)
      return false;

    m_running= false;

    NdbThread_WaitFor(m_thread, &res);
    NdbThread_Destroy(&m_thread);
    return true;

  }
  bool is_stopped() { return !m_running; };
};

#endif
 