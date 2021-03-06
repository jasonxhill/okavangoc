//=============================================================================
// StringBufferTest.c
//=============================================================================
#include "TestUtil.h"
#include "../src/StringBuffer.h"
//-----------------------------------------------------------------------------
static void testAppendString();
static void testAppendChar();
static void testToString();
//-----------------------------------------------------------------------------
void mainStringBufferTests()
{
  TEST(testAppendString);
  TEST(testAppendChar);
  TEST(testToString);
}
//-----------------------------------------------------------------------------
static void testAppendString()
{
  MemoryPool pool = newMemoryPool();

  StringBuffer s = newStringBuffer(&pool, "A test string");
  s.append(&s, " - appended value");
  assertEquals(string, "A test string - appended value", s.str);
  s.append(&s, " & another value");
  assertEquals(string, "A test string - appended value & another value", s.str);
  assertTrue(s.size > strlen("A test string - appended value & another value"));

  pool.drain(&pool);
}
//-----------------------------------------------------------------------------
static void testAppendChar()
{
  MemoryPool pool = newMemoryPool();

  StringBuffer s = newStringBuffer(&pool, "A test string");
  s.appendChar(&s, '!');
  assertEquals(string, "A test string!", s.str);
  s.appendChar(&s, '-');
  assertEquals(string, "A test string!-", s.str);

  pool.drain(&pool);
}
//-----------------------------------------------------------------------------
static void testToString()
{
  MemoryPool pool = newMemoryPool();

  StringBuffer sb = newStringBuffer(&pool, "A test strin");
  sb.appendChar(&sb, 'g');
  assertEquals(string, "A test string", sb.str);

  string s = sb.toString(&sb, &pool);
  assertEquals(string, "A test string", s);
  assertEquals(unsigned_int, 13, strlen(s));

  pool.drain(&pool);
}
//-----------------------------------------------------------------------------