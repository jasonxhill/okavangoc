//=============================================================================
// AutoReleasePool.c
//=============================================================================
#include "AutoReleasePool.h"
#include <string.h>
//-----------------------------------------------------------------------------
static unsigned int allocationCount = 0;

unsigned int getAllocationCount()
{
  return allocationCount;
}
//-----------------------------------------------------------------------------
STRUCT AutoReleasePoolNode {
  struct AutoReleasePoolNode* next;
} AutoReleasePoolNode;
//-----------------------------------------------------------------------------
static void addNode(AutoReleasePoolNode** nodeList, AutoReleasePoolNode* const newNode)
{
  if(*nodeList == NULL)
    *nodeList = newNode;
  else
    addNode(&((*nodeList)->next), newNode);
}
//-----------------------------------------------------------------------------
static void freeNodes(AutoReleasePoolNode* node)
{
  if(node == NULL)
    return;

  AutoReleasePoolNode* const next = node->next;
  free(node);
  allocationCount--;
  freeNodes(next);
}
//-----------------------------------------------------------------------------
static void* autoReleasePoolCalloc(struct AutoReleasePool* const pool, const size_t num, size_t size)
{
  const unsigned int allocSize = sizeof(AutoReleasePoolNode) + num*size;
  void* const mem = memset(malloc(allocSize), 0, allocSize);
  allocationCount++;
  addNode((AutoReleasePoolNode**) &(pool->head), mem);
  return mem + sizeof(AutoReleasePoolNode);
}
//-----------------------------------------------------------------------------
static void drainAutoReleasePool(AutoReleasePool* const this)
{
  freeNodes(this->head);
}
//-----------------------------------------------------------------------------
AutoReleasePool newAutoReleasePool()
{
  AutoReleasePool pool = {
    .head = NULL,
    .calloc = &autoReleasePoolCalloc,
    .drain = &drainAutoReleasePool,
  };

  return pool;
}
//-----------------------------------------------------------------------------