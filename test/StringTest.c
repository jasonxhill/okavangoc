//=============================================================================
// StringTest.c
//=============================================================================
#include "TestUtil.h"
//-----------------------------------------------------------------------------
static void testCopyString();
static void testJoinStrings();
//-----------------------------------------------------------------------------
void mainStringTests()
{
  TEST(testCopyString);
  TEST(testJoinStrings);
}
//-----------------------------------------------------------------------------
static void testCopyString()
{
  MemoryPool pool = newMemoryPool();

  {
    string s = copyString(&pool, "A test string", 5);
    assertEquals(string, "A tes", s);
  }
  {
    string s = copyString(&pool, "A test string", 13);
    assertEquals(string, "A test string", s);
  }

  pool.drain(&pool);
}
//-----------------------------------------------------------------------------
static void testJoinStrings()
{
  MemoryPool pool = newMemoryPool();

  string str = joinStrings(&pool, "Test str A ", "Test str B");
  assertEquals(string, "Test str A Test str B", str);
  assertEquals(unsigned_int, 21, strlen(str));

  pool.drain(&pool);
}
//-----------------------------------------------------------------------------