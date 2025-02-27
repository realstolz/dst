/* Copyright (c) 2014, 2018, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; version 2 of the
   License.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
   02110-1301 USA */

#ifndef RPL_SESSION_H
#define RPL_SESSION_H

#include "my_global.h"

class Gtid_set;
class Sid_map;
class THD;


/**
   This class is an interface for session consistency instrumentation
   in the server. It holds the context information for a given session.

   It does not require locking since access to this content is mutually
   exclusive by design (only one thread reading or writing to this object
   at a time).
 */
class Session_consistency_gtids_ctx
{
public:
  /**
   This is an interface to be implemented by classes that want to listen
   to changes to this context. This can be used, for instance, by the
   session tracker gtids to become aware of ctx modifications.
   */
  class Ctx_change_listener
  {
  public:
    Ctx_change_listener() {}
    virtual void notify_session_gtids_ctx_change()= 0;
  private:
    // not implemented
    Ctx_change_listener(const Ctx_change_listener& rsc);
    Ctx_change_listener& operator=(const Ctx_change_listener& rsc);
  };

private:

  /*
   Local sid_map to enable a lock free m_gtid_set.
   */
  Sid_map* m_sid_map;

  /**
    Set holding the transaction identifiers of the gtids
    to reply back on the response packet.

    Lifecycle: Emptied after the reply is sent back to the application. Remains
    empty until:
    - a RW transaction commits and a GTID is written to the binary log.
    - a RO transaction is issued, the consistency level is set to "Check
      Potential Writes" and the transaction is committed.
  */
  Gtid_set* m_gtid_set;

  /**
   If a listener is registered, e.g., the session track gtids, then this
   points to an instance of such listener.

   Since this context is valid only for one session, there is no need
   to protect this with locks.
  */
  Session_consistency_gtids_ctx::Ctx_change_listener* m_listener;

  /**
   Keeps track of the current session track gtids, so that we capture
   according to what was set before. For instance, if the user does:
   SET @@SESSION.SESSION_TRACK_GTIDS='ALL_GTIDS';
   ...
   SET @@SESSION.SESSION_TRACK_GTIDS='OWN_GTID';

   The last statement should return a set of GTIDs.
  */
  ulong m_curr_session_track_gtids;
  
protected:

  /*
     Auxiliary function to determine if GTID collection should take place
     when it is invoked. It takes into consideration the gtid_mode and
     the current session context.

     @param thd the thread context.
     @return true if should collect gtids, false otherwise.
   */
  inline bool shall_collect(const THD* thd);

  /**
   Auxiliary function that allows notification of ctx change listeners.
   */
  inline void notify_ctx_change_listener()
  {
    m_listener->notify_session_gtids_ctx_change();
  }

public:

  /**
    Simple constructor.
  */
  Session_consistency_gtids_ctx();

  /**
    The destructor. Deletes the m_gtid_set and the sid_map.
  */
  virtual ~Session_consistency_gtids_ctx();

  /**
   Registers the listener. The pointer MUST not be NULL.

   @param listener a pointer to the listener to register.
   @param thd THD context associated to this listener.
  */
  void register_ctx_change_listener(
    Session_consistency_gtids_ctx::Ctx_change_listener* listener,
    THD* thd);

  /**
   Unregisters the listener. The listener MUST have registered previously.

   @param listener a pointer to the listener to register.
  */
  void unregister_ctx_change_listener(
    Session_consistency_gtids_ctx::Ctx_change_listener* listener);

  /**
    This member function MUST return a reference to the set of collected
    GTIDs so far.

    @return the set of collected GTIDs so far.
   */
  inline Gtid_set* state() { return m_gtid_set; }

  /**
     This function MUST be called after the response packet is set to the
     client connected. The implementation may act on the collected state
     for instance to do garbage collection.

     @param thd The thread context.
   * @return true on error, false otherwise.
   */
  virtual bool notify_after_response_packet(const THD* thd);

  /**
     This function SHALL be called once the GTID for the given transaction has
     has been added to GTID_EXECUTED.

     This function SHALL store the data if the thd->variables.session_track_gtids
     is set to a value other than NONE.

     @param thd   The thread context.
     @return true on error, false otherwise.
   */
  virtual bool notify_after_gtid_executed_update(const THD *thd);

  /**
     This function MUST be called after a transaction is committed
     in the server. It should be called regardless whether it is a
     RO or RW transaction. Also, DDLs, DDS are considered transaction
     for what is worth.

     This function SHALL store relevant data for the session consistency.

     @param thd    The thread context.
     @return true on error, false otherwise.
   */
  virtual bool notify_after_transaction_commit(const THD* thd);

  virtual bool notify_after_xa_prepare(const THD* thd)
  {
    return notify_after_transaction_commit(thd);
  }


private:
  // not implemented
  Session_consistency_gtids_ctx(const Session_consistency_gtids_ctx& rsc);
  Session_consistency_gtids_ctx& operator=(const Session_consistency_gtids_ctx& rsc);
};

/*
  This object encapsulates the state kept between transactions of the same client in
  order to compute logical timestamps based on WRITESET_SESSION.
*/
class Dependency_tracker_ctx
{
public:
  Dependency_tracker_ctx(): m_last_session_sequence_number(0) { }

  void set_last_session_sequence_number(int64 sequence_number)
  {
    m_last_session_sequence_number= sequence_number;
  }

  int64 get_last_session_sequence_number()
  {
    return m_last_session_sequence_number;
  }

private:
  int64 m_last_session_sequence_number;
};

/*
  This class SHALL encapsulate the replication context associated with the THD
  object.
 */
class Rpl_thd_context
{
private:
  Session_consistency_gtids_ctx m_session_gtids_ctx;
  Dependency_tracker_ctx m_dependency_tracker_ctx;

  // make these private
  Rpl_thd_context(const Rpl_thd_context& rsc);
  Rpl_thd_context& operator=(const Rpl_thd_context& rsc);
public:

  Rpl_thd_context() { }

  inline Session_consistency_gtids_ctx& session_gtids_ctx()
  {
    return m_session_gtids_ctx;
  }

  inline Dependency_tracker_ctx& dependency_tracker_ctx()
  {
    return m_dependency_tracker_ctx;
  }
};

#endif	/* RPL_SESSION_H */
 