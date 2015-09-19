//=============================================================================
// StringTest.c
//=============================================================================
#include "TestUtil.h"
//-----------------------------------------------------------------------------
static void testCopyString();
static void testAppendString();
static void testTrimStringToSize();
static void testJoinStrings();
//-----------------------------------------------------------------------------
void mainStringTests()
{
  TEST(testCopyString);
  TEST(testAppendString);
  TEST(testTrimStringToSize);
  TEST(testJoinStrings);
}
//-----------------------------------------------------------------------------
static void testCopyString()
{
  AutoReleasePool pool = newAutoReleasePool();

  {
    String s = StringOf("A test string");
    assertEquals(string, "A test string", s.str);
  }
  {
    String s = copyString(&pool, StringOf("A test string"), 5);
    assertEquals(string, "A tes", s.str);
  }
  {
    String s = copyString(&pool, StringOf("A test string"), 13);
    assertEquals(string, "A test string", s.str);
  }

  pool.drain(&pool);
}
//-----------------------------------------------------------------------------
static void testAppendString()
{
  AutoReleasePool pool = newAutoReleasePool();

  String s = StringOf("A test string");
  s = appendChar(&pool, s, '!', 10);
  assertEquals(string, "A test string!", s.str);
  s = appendChar(&pool, s, '-', 10);
  assertEquals(string, "A test string!-", s.str);

  pool.drain(&pool);
}
//-----------------------------------------------------------------------------
static void testTrimStringToSize()
{
  AutoReleasePool pool = newAutoReleasePool();

  String s = copyString(&pool, StringOf("A test string"), 50);
  assertEquals(string, "A test string", s.str);
  assertEquals(unsigned_int, 50, s.size);

  s = trimStringToSize(&pool, s);
  assertEquals(string, "A test string", s.str);
  assertEquals(unsigned_int, 13, s.size);

  pool.drain(&pool);
}
//-----------------------------------------------------------------------------
static void testJoinStrings()
{
  AutoReleasePool pool = newAutoReleasePool();

  {
    string str = joinstrings(&pool, "Test str A ", "Test str B");
    assertEquals(string, "Test str A Test str B", str);
    assertEquals(unsigned_int, 21, strlen(str));
  }

  {
    String str = joinStrings(&pool, StringOf("Test part A "), StringOf("Test part B"));
    assertEquals(string, "Test part A Test part B", str.str);
    assertEquals(unsigned_int, 23, str.size);
  }

  pool.drain(&pool);
}
//-----------------------------------------------------------------------------