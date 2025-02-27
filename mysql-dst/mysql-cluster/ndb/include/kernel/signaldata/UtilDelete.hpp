/*
   Copyright (c) 2003, 2013, Oracle and/or its affiliates. All rights reserved.

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

#ifndef UTIL_DELETE_HPP
#define UTIL_DELETE_HPP

#include "SignalData.hpp"
#include <SimpleProperties.hpp>

#define JAM_FILE_ID 124


/**
 * UTIL_DELETE_REQ, UTIL_DELETE_CONF, UTIL_DELETE_REF
 */

/**
 * @class UtilDeleteReq
 * @brief Delete transaction in Util block
 *
 * Data format:
 * - UTIL_DELETE_REQ <prepareId> <ListOfAttributeHeaderValuePairs>
 */

class UtilDeleteReq {
  /** Sender(s) / Receiver(s) */
  friend class DbUtil;

  /** For printing */
  friend bool printUTIL_DELETE_REQ(FILE * output, const Uint32 * theData, 
				   Uint32 len, Uint16 receiverBlockNo);
public:
  STATIC_CONST( DataLength = 22 );
  STATIC_CONST( HeaderLength = 3 );

private:  
  Uint32 senderData;
  Uint32 prepareId;     // Which prepared transaction to execute
  Uint32 totalDataLen;  // Total length of attrData (including AttributeHeaders
                        // and possibly spanning over multiple signals)

  /**
   * Length in this = signal->length() - 3
   * Sender block ref = signal->senderBlockRef()
   */
  
  Uint32 attrData[DataLength];
};



/**
 * @class UtilDeleteConf
 *
 * Data format:
 * - UTIL_PREPARE_CONF <UtilPrepareId> 
 */

class UtilDeleteConf {
  /**
   * Sender(s) / Receiver(s)
   */
  friend class DbUtil;

  /**
   * For printing
   */
  friend bool printUTIL_DELETE_CONF(FILE * output, 
				     const Uint32 * theData, 
				     Uint32 len, 
				     Uint16 receiverBlockNo);

  STATIC_CONST( SignalLength = 1 );

private:
  Uint32 senderData;   ///< The client data provided by the client sending
                       ///< UTIL_DELETE_REQ
};


/**
 * @class UtilDeleteRef
 *
 * Data format:
 * - UTIL_PREPARE_REF 
 */

class UtilDeleteRef {
  /**
   * Sender(s) / Receiver(s)
   */
  friend class DbUtil;

  /**
   * For printing
   */
  friend bool printUTIL_DELETE_REF(FILE * output, 
				    const Uint32 * theData, 
				    Uint32 len, 
				    Uint16 receiverBlockNo);

  STATIC_CONST( SignalLength = 2 );

private:
  Uint32 senderData;
  Uint32 errorCode;   ///< See UtilExecuteRef::errorCode
  Uint32 TCErrorCode;
};



#undef JAM_FILE_ID

#endif
 