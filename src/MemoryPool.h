//=============================================================================
// MemoryPool.h
//=============================================================================
#ifndef SRC_MEMORYPOOL_H_
#define SRC_MEMORYPOOL_H_

#include "Util.h"
//-----------------------------------------------------------------------------
STRUCT MemoryPool {
  void* head;
  void* (* const calloc)(struct MemoryPool*, size_t, size_t);
  void (* const drain)(struct MemoryPool*);
} MemoryPool;
//-----------------------------------------------------------------------------
MemoryPool newMemoryPool();
//-----------------------------------------------------------------------------
unsigned int getAllocationCount();
//-----------------------------------------------------------------------------
#endif // SRC_MEMORYPOOL_H_
//-----------------------------------------------------------------------------