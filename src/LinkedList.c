//=============================================================================
// LinkedList.c
//=============================================================================
#include "LinkedList.h"
//-----------------------------------------------------------------------------
STRUCT LinkedListNode {
  struct LinkedListNode* previous;
  struct LinkedListNode* next;
  void* value;
} LinkedListNode;
//-----------------------------------------------------------------------------
static LinkedList appendToList(LinkedList*, void*);
static void appendToNode(LinkedListNode*, void*, MemoryPool*);
//-----------------------------------------------------------------------------
static void* getHead(LinkedList*);
static LinkedList getTail(LinkedList*);
static LinkedList getUntail(LinkedList*);
//-----------------------------------------------------------------------------
static LinkedList getFirst(LinkedList*);
static LinkedList getLast(LinkedList*);
//-----------------------------------------------------------------------------
static BOOL isEmpty(const LinkedList*);
static BOOL isUntailable(const LinkedList*);
//-----------------------------------------------------------------------------
static unsigned int getSize(const LinkedList*);
static unsigned int getNodeCount(const LinkedListNode*);
//-----------------------------------------------------------------------------
LinkedList newLinkedList(MemoryPool* const pool)
{
  LinkedList list = {
    .data = pool->calloc(pool, 1, sizeof(LinkedListNode)),
    .append = &appendToList,

    .head = &getHead,
    .tail = &getTail,
    .untail = &getUntail,

    .first = &getFirst,
    .last = &getLast,

    .empty = &isEmpty,
    .untailable = &isUntailable,

    .size = &getSize,
    .memoryPool = pool
  };

  return list;
}
//-----------------------------------------------------------------------------
#define ROOT ((LinkedListNode*)list->data)
#define FIRST (ROOT->next)
//-----------------------------------------------------------------------------
static LinkedList appendToList(LinkedList* const list, void* const value)
{
  appendToNode(ROOT, value, list->memoryPool);
  return *list;
}
//-----------------------------------------------------------------------------
static void appendToNode(LinkedListNode* const previous,
                         void* const value,
                         MemoryPool* const pool)
{
  if(previous->next != NULL)
  {
    appendToNode(previous->next, value, pool);
    return;
  }

  previous->next = pool->calloc(pool, 1, sizeof(LinkedListNode));
  previous->next->value = value;
  previous->next->previous = previous;
}
//-----------------------------------------------------------------------------
static void* getHead(LinkedList* const list)
{
  LinkedListNode* const first = FIRST;
  return first != NULL? first->value : NULL;
}
//-----------------------------------------------------------------------------
static LinkedList getTail(LinkedList* const list)
{
  if(FIRST == NULL)
    return *list;

  LinkedList tail = *list;
  tail.data = FIRST;
  return tail;
}
//-----------------------------------------------------------------------------
static LinkedList getUntail(LinkedList* const list)
{
  if(ROOT->previous == NULL)
    return *list;

  LinkedList previous = *list;
  previous.data = ROOT->previous;
  return previous;
}
//-----------------------------------------------------------------------------
static LinkedList getFirst(LinkedList* const list)
{
  if(ROOT->previous == NULL)
    return *list;

  LinkedList tail = getUntail(list);
  return getFirst(&tail);
}
//-----------------------------------------------------------------------------
static LinkedList getLast(LinkedList* const list)
{
  if(FIRST == NULL || FIRST->next == NULL)
    return *list;

  LinkedList tail = getTail(list);
  return getLast(&tail);
}
//-----------------------------------------------------------------------------
static BOOL isEmpty(const LinkedList* const list)
{
  return FIRST == NULL;
}
//-----------------------------------------------------------------------------
static BOOL isUntailable(const LinkedList* const list)
{
  return ROOT->previous != NULL;
}
//-----------------------------------------------------------------------------
static unsigned int getSize(const LinkedList* const list)
{
  return getNodeCount(FIRST);
}
//-----------------------------------------------------------------------------
static unsigned int getNodeCount(const LinkedListNode* const node)
{
  return node != NULL? getNodeCount(node->next) + 1 : 0;
}
//-----------------------------------------------------------------------------