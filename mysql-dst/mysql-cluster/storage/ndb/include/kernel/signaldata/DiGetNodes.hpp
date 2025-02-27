/*
   Copyright (c) 2003, 2016, Oracle and/or its affiliates. All rights reserved.

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

#ifndef DIGETNODES_HPP
#define DIGETNODES_HPP

#include <NodeBitmask.hpp>
#include <ndb_limits.h>

#define JAM_FILE_ID 90


/**
 * 
 */
struct DiGetNodesConf {
  /**
   * Receiver(s)
   */
  friend class Dbtc;
  friend class Dbspj;
  friend class Backup;
  friend class Suma;

  /**
   * Sender(s)
   */
  friend class Dbdih;

  STATIC_CONST( SignalLength = 3 + MAX_REPLICAS );
  STATIC_CONST( REORG_MOVING = 0x80000000);

  Uint32 zero;
  Uint32 fragId;
  Uint32 reqinfo;
  Uint32 nodes[MAX_REPLICAS + (2 + MAX_REPLICAS)]; //+1
};
/**
 * 
 */
class DiGetNodesReq {
  /**
   * Sender(s)
   */
  friend class Dbtc;
  friend class Dbspj;
  friend class Backup;
  friend class Suma;
  /**
   * Receiver(s)
   */
  friend class Dbdih;
public:
  STATIC_CONST( SignalLength = 6 + (sizeof(void*) / sizeof(Uint32)) );
  STATIC_CONST(MAX_DIGETNODESREQS = 16);
private:
  Uint32 tableId;
  Uint32 hashValue;
  Uint32 distr_key_indicator;
  Uint32 scan_indicator;
  Uint32 get_next_fragid_indicator;
  Uint32 anyNode;
  union {
    void * jamBufferPtr;
    Uint32 jamBufferStorage[2];
  };
};


#undef JAM_FILE_ID

#endif
 