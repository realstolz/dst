/*
 Copyright (c) 2014, 2016 , Oracle and/or its affiliates. All rights
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

#include <node_buffer.h>

#include "adapter_global.h"
#include "unified_debug.h"
#include "KeyOperation.h"

using namespace v8;

const char * opcode_strings[17] = 
 { 0, "read  ", "insert", 0, "update", 0, 0, 0, "write ",
   0, 0, 0, 0, 0, 0, 0, "delete" };

const char * KeyOperation::getOperationName() {
  return opcode < 17 ? opcode_strings[opcode] : 0;
}

// Call the right destructor for a chain of blob handlers,
// where B is either BlobReadHandler or BlobWriteHandler
template <typename B> void deleteBlobChain(BlobHandler *b) {
  do {
    B * blobHandler = static_cast<B *>(b);
    b = blobHandler->getNext();
    delete blobHandler;
  } while(b);
}

KeyOperation::~KeyOperation() {
  if(isBlobReadOperation()) {
    deleteBlobChain<BlobReadHandler>(blobHandler);
  } else if(blobHandler) {
    deleteBlobChain<BlobWriteHandler>(blobHandler);
  }
}

const NdbOperation * KeyOperation::readTuple(NdbTransaction *tx) {
  const NdbOperation *op;
  op = tx->readTuple(key_record->getNdbRecord(), key_buffer,
                     row_record->getNdbRecord(), row_buffer, lmode, read_mask_ptr);
  if(blobHandler) blobHandler->prepare(op);
  return op;                       
}

const NdbOperation * KeyOperation::deleteTuple(NdbTransaction *tx) {
  return tx->deleteTuple(key_record->getNdbRecord(), key_buffer,
                         row_record->getNdbRecord(), 0, 0, options);
}                         

const NdbOperation * KeyOperation::writeTuple(NdbTransaction *tx) { 
  const NdbOperation *op;
  op = tx->writeTuple(key_record->getNdbRecord(), key_buffer,
                      row_record->getNdbRecord(), row_buffer, u.row_mask);
  if(blobHandler) blobHandler->prepare(op);
  return op;
}

const NdbOperation * KeyOperation::insertTuple(NdbTransaction *tx) { 
  const NdbOperation *op;
  op = tx->insertTuple(row_record->getNdbRecord(), row_buffer,
                       u.row_mask, options);
  if(blobHandler) blobHandler->prepare(op);
  return op;
}

const NdbOperation * KeyOperation::updateTuple(NdbTransaction *tx) { 
  const NdbOperation *op;
  op = tx->updateTuple(key_record->getNdbRecord(), key_buffer,
                       row_record->getNdbRecord(), row_buffer,
                       u.row_mask, options);
  if(blobHandler) blobHandler->prepare(op);
  return op;
}


const NdbOperation * KeyOperation::prepare(NdbTransaction *tx) {
  switch(opcode) {
    case 1:  // OP_READ:
      return readTuple(tx);
    case 2:  // OP_INSERT:
      return insertTuple(tx);
    case 4:  // OP_UPDATE:
      return updateTuple(tx);
    case 8:  // OP_WRITE:
      return writeTuple(tx);
    case 16: // OP_DELETE:
      return deleteTuple(tx);
    default:
      return NULL;
  }
}

void KeyOperation::setBlobHandler(BlobHandler *b) {
  b->setNext(blobHandler);
  blobHandler = b;
}

int KeyOperation::createBlobReadHandles(const Record * rowRecord) {
  DEBUG_MARKER(UDEB_DEBUG);
  int ncreated = 0;
  int ncol = rowRecord->getNoOfColumns();
  for(int i = 0 ; i < ncol ; i++) {
    const NdbDictionary::Column * col = rowRecord->getColumn(i);
    if((col->getType() ==  NdbDictionary::Column::Blob) ||
       (col->getType() ==  NdbDictionary::Column::Text)) 
    {
      setBlobHandler(new BlobReadHandler(i, col->getColumnNo()));
      ncreated++;
    }
  }
  return ncreated;
}


int KeyOperation::createBlobWriteHandles(Handle<Object> blobsArray,
                                         const Record * rowRecord) {
  DEBUG_MARKER(UDEB_DEBUG);
  int ncreated = 0;
  int ncol = rowRecord->getNoOfColumns();
  for(int i = 0 ; i < ncol ; i++) {
    if(blobsArray->Get(i)->IsObject()) {
      Local<Object> blobValue = blobsArray->Get(i)->ToObject();
      assert(node::Buffer::HasInstance(blobValue));
      const NdbDictionary::Column * col = rowRecord->getColumn(i);
      assert( (col->getType() ==  NdbDictionary::Column::Blob) ||
              (col->getType() ==  NdbDictionary::Column::Text));
      ncreated++;
      setBlobHandler(new BlobWriteHandler(i, col->getColumnNo(), blobValue));
    }
  }
  return ncreated;
}


void KeyOperation::readBlobResults(const Arguments & args) {
  DEBUG_MARKER(UDEB_DEBUG);
  v8::Isolate * isolate = args.GetIsolate();
  EscapableHandleScope scope(isolate);

  args.GetReturnValue().SetUndefined();
  if(isBlobReadOperation()) {
    Local<Object> results = Array::New(isolate);
    BlobReadHandler * readHandler = static_cast<BlobReadHandler *>(blobHandler);
    while(readHandler) {
      Local<Value> buffer = readHandler->getResultBuffer(isolate);
      if(buffer.IsEmpty()) {
        buffer = Null(isolate);
      }
      results->ToObject()->Set(readHandler->getFieldNumber(), buffer);
      readHandler = static_cast<BlobReadHandler *>(readHandler->getNext());
    }
    args.GetReturnValue().Set(scope.Escape(results));
  }
}
 