//=============================================================================
// LinkedListTest.c
//=============================================================================
#include "TestUtil.h"
#include "../src/LinkedList.h"
//-----------------------------------------------------------------------------
static void testLinkedList();
//-----------------------------------------------------------------------------
void mainLinkedListTests()
{
  TEST(testLinkedList);
}
//-----------------------------------------------------------------------------
void testLinkedList()
{
  MemoryPool pool = newMemoryPool();

  LinkedList listA = newLinkedList(&pool);
  assertEquals(unsigned_int, 0, listA.size(&listA));

  listA.append(&listA, "string A");
  assertEquals(unsigned_int, 1, listA.size(&listA));

  listA.append(&listA, "string B");
  assertEquals(unsigned_int, 2, listA.size(&listA));

  listA.append(&listA, "string C");
  assertEquals(unsigned_int, 3, listA.size(&listA));

  assertEquals(string, listA.head(&listA), "string A");

  LinkedList listB = listA.tail(&listA);
  assertEquals(unsigned_int, 2, listB.size(&listB));
  assertEquals(string, listB.head(&listB), "string B");

  LinkedList listC = listB.tail(&listB);
  assertEquals(unsigned_int, 1, listC.size(&listC));
  assertEquals(string, listC.head(&listC), "string C");

  listB.append(&listB, "string D");
  assertEquals(unsigned_int, 4, listA.size(&listA));
  assertEquals(unsigned_int, 3, listB.size(&listB));
  assertEquals(unsigned_int, 2, listC.size(&listC));

  LinkedList listD = listC.tail(&listC);
  assertEquals(unsigned_int, 1, listD.size(&listD));
  assertEquals(string, listD.head(&listD), "string D");

  LinkedList listE = listD.tail(&listD);
  assertEquals(unsigned_int, 0, listE.size(&listE));

  listE.append(&listE, "string E");
  assertEquals(unsigned_int, 1, listE.size(&listE));
  assertEquals(string, listE.head(&listE), "string E");

  assertEquals(unsigned_int, 5, listA.size(&listA));
  assertEquals(unsigned_int, 4, listB.size(&listB));
  assertEquals(unsigned_int, 3, listC.size(&listC));

  pool.drain(&pool);
}
//-----------------------------------------------------------------------------