//=============================================================================
// AutoReleasePool.h
//=============================================================================
#ifndef SRC_AUTORELEASEPOOL_H_
#define SRC_AUTORELEASEPOOL_H_

#include "Util.h"
//-----------------------------------------------------------------------------
STRUCT AutoReleasePool {
  void* head;
  void* (* const calloc)(struct AutoReleasePool*, size_t, size_t);
  void (* const drain)(struct AutoReleasePool*);
} AutoReleasePool;
//-----------------------------------------------------------------------------
AutoReleasePool newAutoReleasePool();
//-----------------------------------------------------------------------------
unsigned int getAllocationCount();
//-----------------------------------------------------------------------------
#endif // SRC_AUTORELEASEPOOL_H_
//-----------------------------------------------------------------------------