/*
   Copyright (C) 2003, 2005-2007 MySQL AB
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
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include <signaldata/CreateIndx.hpp>

bool
printCREATE_INDX_REQ(FILE* output, const Uint32* theData, Uint32 len, Uint16)
{
  const CreateIndxReq* sig = (const CreateIndxReq*)theData;
  fprintf(output, " clientRef: 0x%x", sig->clientRef);
  fprintf(output, " clientData: %u", sig->clientData);
  fprintf(output, "\n");
  fprintf(output, " transId: 0x%x", sig->transId);
  fprintf(output, " transKey: %u", sig->transKey);
  fprintf(output, " requestInfo: 0x%x", sig->requestInfo);
  fprintf(output, "\n");
  fprintf(output, " tableId: %u", sig->tableId);
  fprintf(output, " tableVersion: 0x%x", sig->tableVersion);
  fprintf(output, " indexType: %u", sig->indexType);
  fprintf(output, " online: %u", sig->online);
  fprintf(output, "\n");
  return true;
}

bool
printCREATE_INDX_CONF(FILE* output, const Uint32* theData, Uint32 len, Uint16)
{
  const CreateIndxConf* sig = (const CreateIndxConf*)theData;
  fprintf(output, " senderRef: 0x%x", sig->senderRef);
  fprintf(output, " clientData: %u", sig->clientData);
  fprintf(output, " transId: 0x%x", sig->transId);
  fprintf(output, "\n");
  fprintf(output, " indexId: %u", sig->indexId);
  fprintf(output, " indexVersion: 0x%x", sig->indexVersion);
  fprintf(output, "\n");
  return true;
}

bool printCREATE_INDX_REF(FILE* output, const Uint32* theData, Uint32 len, Uint16)
{
  const CreateIndxRef* sig = (const CreateIndxRef*)theData;
  fprintf(output, " senderRef: 0x%x", sig->senderRef);
  fprintf(output, " clientData: %u", sig->clientData);
  fprintf(output, " transId: 0x%x", sig->transId);
  fprintf(output, "\n");
  fprintf(output, " errorCode: %u", sig->errorCode);
  fprintf(output, " errorLine: %u", sig->errorLine);
  fprintf(output, " errorNodeId: %u", sig->errorNodeId);
  fprintf(output, " masterNodeId: %u", sig->masterNodeId);
  fprintf(output, "\n");
  return true;
}
 