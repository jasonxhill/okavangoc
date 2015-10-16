//=============================================================================
// LinkedList.c
//=============================================================================
#include "LinkedList.h"
//-----------------------------------------------------------------------------
STRUCT LinkedListNode {
  struct LinkedListNode* next;
  void* value;
} LinkedListNode;
//-----------------------------------------------------------------------------
static void appendToList(LinkedList*, void*);
static void appendToNode(LinkedListNode**, void*, MemoryPool*);
static void* getHead(LinkedList*);
static LinkedList getTail(LinkedList*);
static unsigned int getSize(LinkedList*);
static unsigned int getNodeCount(LinkedListNode*);
//-----------------------------------------------------------------------------
LinkedList newLinkedList(MemoryPool* const pool)
{
  LinkedList list = {
    .data = pool->calloc(pool, 1, sizeof(LinkedListNode)),
    .append = &appendToList,
    .head = &getHead,
    .tail = &getTail,
    .size = &getSize,
    .memoryPool = pool
  };

  return list;
}
//-----------------------------------------------------------------------------
#define FIRST(LIST) (((LinkedListNode*)LIST->data)->next)
//-----------------------------------------------------------------------------
static void appendToList(LinkedList* const list, void* const value)
{
  appendToNode(&FIRST(list), value, list->memoryPool);
}
//-----------------------------------------------------------------------------
static void appendToNode(LinkedListNode** node, void* const value, MemoryPool* const pool)
{
  if(*node != NULL)
  {
    appendToNode(&((*node)->next), value, pool);
    return;
  }

  LinkedListNode* const nextNode = pool->calloc(pool, 1, sizeof(LinkedListNode));
  nextNode->value = value;
  *node = nextNode;
}
//-----------------------------------------------------------------------------
static void* getHead(LinkedList* const list)
{
  LinkedListNode* const first = FIRST(list);
  return first != NULL? first->value : NULL;
}
//-----------------------------------------------------------------------------
static LinkedList getTail(LinkedList* const list)
{
  if(FIRST(list) == NULL)
    return *list;

  LinkedList tail = *list;
  tail.data = FIRST(list);
  return tail;
}
//-----------------------------------------------------------------------------
static unsigned int getSize(LinkedList* const list)
{
  return getNodeCount(FIRST(list));
}
//-----------------------------------------------------------------------------
static unsigned int getNodeCount(LinkedListNode* const node)
{
  return node != NULL? getNodeCount(node->next) + 1 : 0;
}
//-----------------------------------------------------------------------------