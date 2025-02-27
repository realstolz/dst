/* Copyright (c) 2013, 2018, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   51 Franklin Street, Suite 500, Boston, MA 02110-1335 USA */

#ifndef DEFINED_RPL_BINLOG_SENDER
#define DEFINED_RPL_BINLOG_SENDER

#ifdef HAVE_REPLICATION
#include "my_global.h"
#include "binlog.h"           // LOG_INFO
#include "binlog_event.h"     // enum_binlog_checksum_alg, Log_event_type
#include "m_string.h"
#include "mysqld_error.h"     // ER_*
#include "sql_error.h"        // Diagnostics_area


/**
  The major logic of dump thread is implemented in this class. It sends
  required binlog events to clients according to their requests.
*/
class Binlog_sender
{
public:
  Binlog_sender(THD *thd, const char *start_file, my_off_t start_pos,
                Gtid_set *exclude_gtids, uint32 flag);

  ~Binlog_sender() {}

  /**
    It checks the dump reqest and sends events to the client until it finish
    all events(for mysqlbinlog) or encounters an error.
  */
  void run();
private:
  THD *m_thd;
  String& m_packet;

  /* Requested start binlog file and position */
  const char *m_start_file;
  my_off_t m_start_pos;

  /*
    For COM_BINLOG_DUMP_GTID, It may include a GTID set. All events in the set
    should not be sent to the client.
  */
  Gtid_set *m_exclude_gtid;
  bool m_using_gtid_protocol;
  bool m_check_previous_gtid_event;
  bool m_gtid_clear_fd_created_flag;

  /* The binlog file it is reading */
  LOG_INFO m_linfo;

  binary_log::enum_binlog_checksum_alg m_event_checksum_alg;
  binary_log::enum_binlog_checksum_alg m_slave_checksum_alg;
  ulonglong m_heartbeat_period;
  time_t m_last_event_sent_ts;
  /*
    For mysqlbinlog(server_id is 0), it will stop immediately without waiting
    if it already reads all events.
  */
  bool m_wait_new_events;

  Diagnostics_area m_diag_area;
  char m_errmsg_buf[MYSQL_ERRMSG_SIZE];
  const char *m_errmsg;
  int m_errno;
  /*
    The position of the event it reads most recently is stored. So it can report
    the exact position after where an error happens.

    m_last_file will point to m_info.log_file_name, if it is same to
    m_info.log_file_name. Otherwise the file name is copied to m_last_file_buf
    and m_last_file will point to it.
  */
  char m_last_file_buf[FN_REFLEN];
  const char *m_last_file;
  my_off_t m_last_pos;

  /*
    Needed to be able to evaluate if buffer needs to be resized (shrunk).
  */
  ushort m_half_buffer_size_req_counter;

  /*
   * The size of the buffer next time we shrink it.
   * This variable is updated once everytime we shrink or grow the buffer.
   */
  size_t m_new_shrink_size;

  /*
     Max size of the buffer is 4GB (UINT_MAX32). It is UINT_MAX32 since the
     threshold is set to (@c Log_event::read_log_event):

       max(max_allowed_packet,
           opt_binlog_rows_event_max_size + MAX_LOG_EVENT_HEADER)

     - opt_binlog_rows_event_max_size is defined as an unsigned long,
       thence in theory row events can be bigger than UINT_MAX32.

     - max_allowed_packet is set to MAX_MAX_ALLOWED_PACKET which is in
       turn defined as 1GB (i.e., 1024*1024*1024). (@c Binlog_sender::init()).

     Therefore, anything bigger than UINT_MAX32 is not loadable into the
     packet, thus we set the limit to 4GB (which is the value for UINT_MAX32,
     @c PACKET_MAXIMUM_SIZE).

   */
  const static uint32 PACKET_MAX_SIZE;

  /*
   * After these consecutive times using less than half of the buffer
   * the buffer is shrunk.
   */
  const static ushort PACKET_SHRINK_COUNTER_THRESHOLD;

  /**
   * The minimum size of the buffer.
   */
  const static uint32 PACKET_MIN_SIZE;

  /**
   * How much to grow the buffer each time we need to accommodate more bytes
   * than it currently can hold.
   */
  const static float PACKET_GROW_FACTOR;

  /**
   * The dual of PACKET_GROW_FACTOR. How much to shrink the buffer each time
   * it is deemed to being underused.
   */
  const static float PACKET_SHRINK_FACTOR;

  uint32 m_flag;
  /*
    It is true if any plugin requires to observe the transmission for each event.
    And HOOKs(reserve_header, before_send and after_send) are called when
    transmitting each event. Otherwise, it is false and HOOKs are not called.
  */
  bool m_observe_transmission;

  /* It is true if transmit_start hook is called. If the hook is not called
   * it will be false.
   */
  bool m_transmit_started;
  /*
    It initializes the context, checks if the dump request is valid and
    if binlog status is correct.
  */
  void init();
  void cleanup();
  void init_heartbeat_period();
  void init_checksum_alg();
  /** Check if the requested binlog file and position are valid */
  int check_start_file();
  /** Transform read error numbers to error messages. */
  const char* log_read_error_msg(int error);

  /**
    It dumps a binlog file. Events are read and sent one by one. If it need
    to wait for new events, it will wait after already reading all events in
    the active log file.

    @param[in] log_cache  IO_CACHE of the binlog will be sent
    @param[in] start_pos  Position requested by the slave's IO thread.
                          Only the events after the position are sent.

    @return It returns 0 if succeeds, otherwise 1 is returned.
  */
  my_off_t send_binlog(IO_CACHE *log_cache, my_off_t start_pos);

  /**
    It sends some events in a binlog file to the client.


     @param[in] log_cache  IO_CACHE of the binlog will be sent
     @param[in] end_pos    Only the events before end_pos are sent

     @return It returns 0 if succeeds, otherwise 1 is returned.
  */
  int send_events(IO_CACHE *log_cache, my_off_t end_pos);

  /**
    It gets the end position of the binlog file.

    @param[in] log_cache  IO_CACHE of the binlog will be checked

    @return
      @retval 0  It already arrives the end of the binlog.
      @retval 1  Failed to get binlog position
      @retval >1 Succeeded to get binlog end position
  */
  my_off_t get_binlog_end_pos(IO_CACHE *log_cache);

  /**
     It checks if a binlog file has Previous_gtid_log_event

     @param[in]  log_cache  IO_CACHE of the binlog will be checked
     @param[out] found      Found Previous_gtid_log_event or not

     @return It returns 0 if succeeds, otherwise 1 is returned.
  */
  int has_previous_gtid_log_event(IO_CACHE *log_cache, bool *found);

  /**
    It sends a faked rotate event which does not exist physically in any
    binlog to the slave. It contains the name of the binlog we are going to
    send to the slave.

    Faked rotate event is required in a few cases, so slave can know which
    binlog the following events are from.

  - The binlog file slave requested is Empty. E.g.
    "CHANGE MASTER TO MASTER_LOG_FILE='', MASTER_LOG_POS=4", etc.

  - The position slave requested is exactly the end of a binlog file.

  - Previous binlog file does not include a rotate event.
    It happens when server is shutdown and restarted.

  - The previous binary log was GTID-free (does not contain a
    Previous_gtids_log_event) and the slave is connecting using
    the GTID protocol.

    @param[in] packet         The buffer used to store the faked event.
    @param[in] next_log_file  The name of the binlog file will be sent after
                              the rotate event.
    @param[in] log_pos        The start position of the binlog file.

    @return It returns 0 if succeeds, otherwise 1 is returned.
  */
  int fake_rotate_event(const char *next_log_file, my_off_t log_pos);

  /**
     When starting to dump a binlog file, Format_description_log_event
     is read and sent first. If the requested position is after
     Format_description_log_event, log_pos field in the first
     Format_description_log_event has to be set to 0. So the slave
     will not increment its master's binlog position.

     @param[in] log_cache IO_CACHE of the binlog will be dumpped
     @param[in] start_pos Position requested by the slave's IO thread.
                          Only the events after the position are sent.

     @return It returns 0 if succeeds, otherwise 1 is returned.
  */
  int send_format_description_event(IO_CACHE *log, my_off_t start_pos);
  /**
     It sends a heartbeat to the client.

     @param[in] packet   The buffer used to store the event.
     @param[in] log_pos  The log position that events before it are sent.

     @return It returns 0 if succeeds, otherwise 1 is returned.
  */
  int send_heartbeat_event(my_off_t log_pos);

  /**
     It reads a event from binlog file.

     @param[in] log_cache     IO_CACHE of the binlog file.
     @param[in] checksum_alg  Checksum algorithm used to check the event.
     @param[out] event_ptr    The buffer used to store the event.
     @param[out] event_len    Length of the event.

     @return It returns 0 if succeeds, otherwise 1 is returned.
  */
  inline int read_event(IO_CACHE *log_cache,
                        binary_log::enum_binlog_checksum_alg checksum_alg,
                        uchar **event_ptr, uint32 *event_len);
  /**
    Check if it is allowed to send this event type.

    The following are disallowed:
    - GTID_MODE=ON and type==ANONYMOUS_GTID_LOG_EVENT
    - AUTO_POSITION=1 and type==ANONYMOUS_GTID_LOG_EVENT
    - GTID_MODE=OFF and type==GTID_LOG_EVENT

    @param type The event type.
    @param log_file The binary log file (used in error messages).
    @param log_pos The binary log position (used in error messages).

    @retval true The event is not allowed. In this case, this function
    calls set_fatal_error().
    @retval false The event is allowed.
  */
  bool check_event_type(binary_log::Log_event_type type,
                        const char *log_file, my_off_t log_pos);
  /**
    It checks if the event is in m_exclude_gtid.

    Clients may request to exclude some GTIDs. The events include in the GTID
    groups will be skipped. We call below events sequence as a goup,
    Gtid_log_event
    BEGIN
    ...
    COMMIT or ROLLBACK

    or
    Gtid_log_event
    DDL statement

    @param[in] event_ptr  Buffer of the event
    @param[in] event_len  Length of the event
    @param[in] in_exclude_group  If it is in a execude group

    @return It returns true if it should be skipped, otherwise false is turned.
  */
  inline bool skip_event(const uchar *event_ptr, uint32 event_len,
                         bool in_exclude_group);

  inline void calc_event_checksum(uchar *event_ptr, size_t event_len);
  inline int flush_net();
  inline int send_packet();
  inline int send_packet_and_flush();
  inline int before_send_hook(const char *log_file, my_off_t log_pos);
  inline int after_send_hook(const char *log_file, my_off_t log_pos);
  /*
    Reset the thread transmit packet buffer for event sending.

    This function reserves the bytes for event transmission, and
    should be called before storing the event data to the packet buffer.

    @param[in] flags      The flag used in reset_transmit hook.
    @param[in] event_len  If the caller already knows the event length, then
                          it can pass this value so that reset_transmit_packet
                          already reallocates the buffer if needed. Otherwise,
                          if event_len is 0, then the caller needs to extend
                          the buffer itself.
  */
  inline int reset_transmit_packet(ushort flags, size_t event_len= 0);

  /**
    It waits until receiving an update_cond signal. It will send heartbeat
    periodically if m_heartbeat_period is set.

    @param[in] log_pos  The end position of the last event it already sent.
    It is required by heartbeat events.

    @return It returns 0 if succeeds, otherwise 1 is returned.
  */
  inline int wait_new_events(my_off_t log_pos);
  inline int wait_with_heartbeat(my_off_t log_pos);
  inline int wait_without_heartbeat();

#ifndef DBUG_OFF
  /* It is used to count the events that have been sent. */
  int m_event_count;
  /*
    It aborts dump thread with an error if m_event_count exceeds
    max_binlog_dump_events.
  */
  inline int check_event_count();
#endif

  bool has_error() { return m_errno != 0; }
  void set_error(int errorno, const char *errmsg)
  {
    my_snprintf(m_errmsg_buf, sizeof(m_errmsg_buf), "%.*s",
                MYSQL_ERRMSG_SIZE - 1, errmsg);
    m_errmsg= m_errmsg_buf;
    m_errno= errorno;
  }

  void set_unknow_error(const char *errmsg)
  {
    set_error(ER_UNKNOWN_ERROR, errmsg);
  }

  void set_fatal_error(const char *errmsg)
  {
    set_error(ER_MASTER_FATAL_ERROR_READING_BINLOG, errmsg);
  }

  bool is_fatal_error()
  {
    return m_errno == ER_MASTER_FATAL_ERROR_READING_BINLOG;
  }

  bool event_checksum_on()
  {
    return m_event_checksum_alg > binary_log::BINLOG_CHECKSUM_ALG_OFF &&
      m_event_checksum_alg < binary_log::BINLOG_CHECKSUM_ALG_ENUM_END;
  }

  void set_last_pos(my_off_t log_pos)
  {
    m_last_file= m_linfo.log_file_name;
    m_last_pos= log_pos;
  }

  void set_last_file(const char *log_file)
  {
    strcpy(m_last_file_buf, log_file);
    m_last_file= m_last_file_buf;
  }

  /**
   * This function SHALL grow the buffer of the packet if needed.
   *
   * If the buffer used for the packet is large enough to accommodate
   * the requested extra bytes, then this function does not do anything.
   *
   * On the other hand, if the requested size is bigger than the available
   * free bytes in the buffer, the buffer is extended by a constant factor
   * (@c PACKET_GROW_FACTOR).
   *
   * @param packet  The buffer to resize if needed.
   * @param extra_size  The size in bytes that the caller wants to add to the buffer.
   * @return true if an error occurred, false otherwise.
   */
  inline bool grow_packet(size_t extra_size);

  /**
   * This function SHALL shrink the size of the buffer used.
   *
   * If less than half of the buffer was used in the last N
   * (@c PACKET_SHRINK_COUNTER_THRESHOLD) consecutive times this function
   * was called, then the buffer gets shrunk by a constant factor
   * (@c PACKET_SHRINK_FACTOR).
   *
   * The buffer is never shrunk less than a minimum size (@c PACKET_MIN_SIZE).
   *
   * @param packet  The buffer to shrink.
   */
  inline bool shrink_packet();

  /**
   Helper function to recalculate a new size for the growing buffer.

   @param current_size The baseline (for instance, the current buffer size).
   @param min_size The resulting buffer size, needs to be at least as large
                   as this parameter states.
   @return The new buffer size, or 0 in the case of an error.
  */
  inline size_t calc_grow_buffer_size(size_t current_size, size_t min_size);

  /**
   Helper function to recalculate the new size for the m_new_shrink_size.

   @param current_size The baseline (for instance, the current buffer size).
  */
  void calc_shrink_buffer_size(size_t current_size);
};

#endif // HAVE_REPLICATION
#endif // DEFINED_RPL_BINLOG_SENDER
 