//=============================================================================
// StringBuffer.h
//=============================================================================
#ifndef SRC_STRING_BUFFER_H_
#define SRC_STRING_BUFFER_H_

#include "MemoryPool.h"
#include "String.h"
//-----------------------------------------------------------------------------
STRUCT StringBuffer {
  string str;
  unsigned int size;
  struct StringBuffer (*appendChar)(struct StringBuffer*, CHAR);
  string (*toString)(struct StringBuffer*, MemoryPool*);
  MemoryPool* memoryPool;
} StringBuffer;
//-----------------------------------------------------------------------------
StringBuffer newStringBuffer(MemoryPool*, string);
//-----------------------------------------------------------------------------
#endif // SRC_STRING_BUFFER_H_
//-----------------------------------------------------------------------------