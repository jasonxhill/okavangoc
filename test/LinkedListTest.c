//=============================================================================
// LinkedListTest.c
//=============================================================================
#include "TestUtil.h"
#include "../src/LinkedList.h"
#include "../src/String.h"
//-----------------------------------------------------------------------------
static void testLinkedList();
static void testLinkedListTraversal();
static void testLinkedListToArray();
//-----------------------------------------------------------------------------
void mainLinkedListTests()
{
  TEST(testLinkedList);
  TEST(testLinkedListTraversal);
  TEST(testLinkedListToArray);
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
static void testLinkedListTraversal()
{
  MemoryPool pool = newMemoryPool();
  LinkedList list = newLinkedList(&pool);

  assertTrue(list.empty(&list));
  list.append(&list, "valA,");
  assertFalse(list.empty(&list));

  list.append(&list, "valB,")
      .append(&list, "valC,")
      .append(&list, "valD,")
      .append(&list, "valE,");

  LinkedList last = list.last(&list);
  assertEquals(string, "valE,", last.head(&last));

  LinkedList first = last.first(&last);
  assertEquals(string, "valA,", first.head(&first));

  string s = "";

  for(LinkedList l = first; !l.empty(&l); l = l.tail(&l))
    s = joinStrings(&pool, s, l.head(&l));

  assertEquals(string, "valA,valB,valC,valD,valE,", s);

  s = "";

  for(LinkedList l = last; TRUE; l = l.untail(&l)) {
    s = joinStrings(&pool, s, l.head(&l));

    if(!l.untailable(&l))
      break;
  }

  assertEquals(string, "valE,valD,valC,valB,valA,", s);

  pool.drain(&pool);
}
//-----------------------------------------------------------------------------
void testLinkedListToArray()
{
  MemoryPool pool = newMemoryPool();

  LinkedList list = newLinkedList(&pool);
  list.append(&list, "valA,")
      .append(&list, "valB,")
      .append(&list, "valC,")
      .append(&list, "valD,")
      .append(&list, "valE,");

  string result = "";

  for(string* s = (string*) list.toArray(&list, &pool); *s != NULL; s++)
    result = joinStrings(&pool, result, *s);

  assertEquals(string, "valA,valB,valC,valD,valE,", result);

  pool.drain(&pool);
}
//-----------------------------------------------------------------------------