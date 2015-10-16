//=============================================================================
// LinkedList.h
//=============================================================================
#ifndef SRC_LINKEDLIST_H_
#define SRC_LINKEDLIST_H_

#include "AutoReleasePool.h"
//-----------------------------------------------------------------------------
STRUCT LinkedList {
  void* data;
  void (*append)(struct LinkedList*, void*);
  void* (*head)(struct LinkedList*);
  struct LinkedList (*tail)(struct LinkedList*);
  unsigned int (*size)(struct LinkedList*);
  AutoReleasePool* autoReleasePool;
} LinkedList;
//-----------------------------------------------------------------------------
LinkedList newLinkedList(AutoReleasePool*);
//-----------------------------------------------------------------------------
#endif // SRC_LINKEDLIST_H_
//-----------------------------------------------------------------------------