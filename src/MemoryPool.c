//=============================================================================
// MemoryPool.c
//=============================================================================
#include "MemoryPool.h"
#include <string.h>
//-----------------------------------------------------------------------------
static unsigned int allocationCount = 0;

unsigned int getAllocationCount()
{
  return allocationCount;
}
//-----------------------------------------------------------------------------
STRUCT MemoryPoolNode {
  struct MemoryPoolNode* next;
} MemoryPoolNode;
//-----------------------------------------------------------------------------
static void addNode(MemoryPoolNode** nodeList, MemoryPoolNode* const newNode)
{
  if(*nodeList == NULL)
    *nodeList = newNode;
  else
    addNode(&((*nodeList)->next), newNode);
}
//-----------------------------------------------------------------------------
static void freeNodes(MemoryPoolNode* node)
{
  if(node == NULL)
    return;

  MemoryPoolNode* const next = node->next;
  free(node);
  allocationCount--;
  freeNodes(next);
}
//-----------------------------------------------------------------------------
static void* memoryPoolCalloc(struct MemoryPool* const pool, const size_t num, size_t size)
{
  const unsigned int allocSize = sizeof(MemoryPoolNode) + num*size;
  void* const mem = memset(malloc(allocSize), 0, allocSize);
  allocationCount++;
  addNode((MemoryPoolNode**) &(pool->head), mem);
  return mem + sizeof(MemoryPoolNode);
}
//-----------------------------------------------------------------------------
static void drainPool(MemoryPool* const this)
{
  freeNodes(this->head);
}
//-----------------------------------------------------------------------------
MemoryPool newMemoryPool()
{
  MemoryPool pool = {
    .head = NULL,
    .calloc = &memoryPoolCalloc,
    .drain = &drainPool,
  };

  return pool;
}
//-----------------------------------------------------------------------------