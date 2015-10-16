//=============================================================================
// LinkedList.h
//=============================================================================
#ifndef SRC_LINKEDLIST_H_
#define SRC_LINKEDLIST_H_

#include "MemoryPool.h"
//-----------------------------------------------------------------------------
STRUCT LinkedList {
  void* data;
  void (*append)(struct LinkedList*, void*);
  void* (*head)(struct LinkedList*);
  struct LinkedList (*tail)(struct LinkedList*);
  unsigned int (*size)(struct LinkedList*);
  MemoryPool* memoryPool;
} LinkedList;
//-----------------------------------------------------------------------------
LinkedList newLinkedList(MemoryPool*);
//-----------------------------------------------------------------------------
#endif // SRC_LINKEDLIST_H_
//-----------------------------------------------------------------------------