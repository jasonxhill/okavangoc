//=============================================================================
// LinkedList.h
//=============================================================================
#ifndef SRC_LINKEDLIST_H_
#define SRC_LINKEDLIST_H_

#include "MemoryPool.h"
//-----------------------------------------------------------------------------
STRUCT LinkedList {
  void* data;
  struct LinkedList (*append)(struct LinkedList*, void*);

  void* (*head)(struct LinkedList*);
  struct LinkedList (*tail)(struct LinkedList*);
  struct LinkedList (*untail)(struct LinkedList*);

  struct LinkedList (*first)(struct LinkedList*);
  struct LinkedList (*last)(struct LinkedList*);

  BOOL (*empty)(const struct LinkedList*);
  BOOL (*untailable)(const struct LinkedList*);

  unsigned int (*size)(const struct LinkedList*);

  void** (*toArray)(const struct LinkedList*, MemoryPool*);

  MemoryPool* memoryPool;
} LinkedList;
//-----------------------------------------------------------------------------
LinkedList newLinkedList(MemoryPool*);
//-----------------------------------------------------------------------------
#endif // SRC_LINKEDLIST_H_
//-----------------------------------------------------------------------------