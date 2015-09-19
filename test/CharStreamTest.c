//=============================================================================
// CharStreamTest.c
//=============================================================================
#include "TestUtil.h"
#include "../src/CharStream.h"
//-----------------------------------------------------------------------------
static void testInMemoryCharStream();
//-----------------------------------------------------------------------------
void mainCharStreamTests()
{
  TEST(testInMemoryCharStream);
}
//-----------------------------------------------------------------------------
static void testInMemoryCharStream()
{
  InMemoryCharStream istream = newInMemoryCharStream("Hello World!");
  CharStream* const stream = &(istream.stream);

  assertEquals(char, 'H', NEXT(stream));
  assertEquals(char, 'e', NEXT(stream));
  assertEquals(char, 'l', NEXT(stream));
  assertEquals(char, 'l', NEXT(stream));
  assertEquals(char, 'o', NEXT(stream));
  assertEquals(char, ' ', NEXT(stream));
  assertEquals(char, 'W', NEXT(stream));
  assertEquals(char, 'o', NEXT(stream));
  assertEquals(char, 'r', NEXT(stream));
  assertEquals(char, 'l', NEXT(stream));
  assertEquals(char, 'd', NEXT(stream));
  assertEquals(char, '!', NEXT(stream));
  assertEquals(char, END_STREAM, NEXT(stream));
  assertEquals(char, END_STREAM, NEXT(stream));
}
//-----------------------------------------------------------------------------