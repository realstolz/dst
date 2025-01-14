/*
 Copyright (c) 2016, Oracle and/or its affiliates. All rights
 reserved.
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; version 2 of
 the License.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 02110-1301  USA
*/


/*
  NDB Connection Properties

*/ 

var NdbDefaultConnectionProperties = {
  "implementation"      : "ndb",            // This must always be "ndb".

  "ndb_connectstring"   : "localhost:1186", // MySQL Cluster Connect String
  "database"            : "test",           // MySQL Database name

  /* This set of properties configures the use of a mysql server for 
     metadata operations (such as creating tables) */
  "mysql_user"          : "root",
  "mysql_password"      : "",
  "mysql_host"          : "localhost",
  "mysql_port"          : 3306,

  /* The next 3 properties control the behavior when opening a connection. */
  "ndb_connect_retries" : 4,         // if < 0, keep trying forever
  "ndb_connect_delay"   : 5,         // full seconds between connection retries
  "ndb_connect_verbose" : 1,         // enable extra console output

  "linger_on_close_msec": 500,       /* When a client closes a DBConnectionPool,
                                        the underlying connection is kept open 
                                        for this many milliseconds in case 
                                        another client tries to re-open it.
                                     */

  "use_ndb_async_api"   : false,     /* If true, some operations will be
                                        executed using asynchronous calls for
                                        improved concurrency. If false, the
                                        number of operations in transit will be
                                        limited to one per uv worker thread.
                                     */

  "use_mapped_ndb_record" : true,    /* If true, results fetched from the
                                        database remain in NDBAPI buffers and
                                        are accessed using V8 accessors.
                                        If false, results fetched from the 
                                        database are copied out of these buffers
                                        into plain old JavaScript objects.
                                     */
  "ndb_session_pool_min" : 4,
  "ndb_session_pool_max" : 100,      /* Each NdbConnectionPool maintains a
                                        pool of DBSessions (and their underlying
                                        Ndb objects).  These parameters set 
                                        guidelines for the size of that pool.
                                     */

  "ndb_session_concurrency" : 4      /* The number of concurrent transactions 
                                        in an Ndb Session.  Only one 
                                        transaction at a time is visible to the
                                        user, but one may start before previous
                                        ones have finished executing.
                                     */
};

/* This file is valid JavaScript 
*/
module.exports = NdbDefaultConnectionProperties;
 