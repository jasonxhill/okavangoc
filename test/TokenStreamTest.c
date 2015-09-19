//=============================================================================
// TokenStreamTest.c
//=============================================================================
#include "TestUtil.h"
#include "../src/TokenStream.h"
#include <assert.h>
//-----------------------------------------------------------------------------
static void testTokenStreamEOF();
static void testTokenStream();
static void testQuotedString();
//-----------------------------------------------------------------------------
void mainTokenStreamTests()
{
  TEST(testTokenStreamEOF);
  TEST(testTokenStream);
  TEST(testQuotedString);
}
//-----------------------------------------------------------------------------
static void testTokenStreamEOF()
{
  {
    AutoReleasePool pool = newAutoReleasePool();

    InMemoryCharStream istream = newInMemoryCharStream("a !! test stream");
    TokenStream stream = newTokenStream(&pool, &(istream.stream));
    String s;

    int i = 0;

    while((s = stream.next(&stream)).str[0] != END_STREAM)
      assert(i++ < 10);

    pool.drain(&pool);
  }
}
//-----------------------------------------------------------------------------
static void testTokenStream()
{
  AutoReleasePool pool = newAutoReleasePool();

  {
    InMemoryCharStream istream = newInMemoryCharStream("a test stream 100");
    TokenStream stream = newTokenStream(&pool, &(istream.stream));
    assertEquals(string, "a", stream.next(&stream).str);
    assertEquals(string, "test", stream.next(&stream).str);
    assertEquals(string, "stream", stream.next(&stream).str);
    assertEquals(string, "100", stream.next(&stream).str);
    assertEquals(string, "", stream.next(&stream).str);
    assertEquals(string, "", stream.next(&stream).str);
  }
  {
    InMemoryCharStream istream = newInMemoryCharStream(" a & test ^^ stream1 100.0f}");
    TokenStream stream = newTokenStream(&pool, &(istream.stream));
    assertEquals(string, "a", stream.next(&stream).str);
    assertEquals(string, "&", stream.next(&stream).str);
    assertEquals(string, "test", stream.next(&stream).str);
    assertEquals(string, "^^", stream.next(&stream).str);
    assertEquals(string, "stream1", stream.next(&stream).str);
    assertEquals(string, "100.0f", stream.next(&stream).str);
    assertEquals(string, "}", stream.next(&stream).str);
    assertEquals(string, "", stream.next(&stream).str);
    assertEquals(string, "", stream.next(&stream).str);
  }
  {
    InMemoryCharStream istream = newInMemoryCharStream(" a $test^\t1stream\n\r} ");
    TokenStream stream = newTokenStream(&pool, &(istream.stream));
    assertEquals(string, "a", stream.next(&stream).str);
    assertEquals(string, "$test", stream.next(&stream).str);
    assertEquals(string, "^", stream.next(&stream).str);
    assertEquals(string, "1stream", stream.next(&stream).str);
    assertEquals(string, "}", stream.next(&stream).str);
    assertEquals(string, "", stream.next(&stream).str);
    assertEquals(string, "", stream.next(&stream).str);
  }
  {
    InMemoryCharStream istream = newInMemoryCharStream("ABC_DE $$ ABC_DE$$ ");
    TokenStream stream = newTokenStream(&pool, &(istream.stream));
    assertEquals(string, "ABC_DE", stream.next(&stream).str);
    assertEquals(string, "$$", stream.next(&stream).str);
    assertEquals(string, "ABC_DE$$", stream.next(&stream).str);
    assertEquals(string, "", stream.next(&stream).str);
  }
  {
    InMemoryCharStream istream = newInMemoryCharStream("GH.J->u.u.r ");
    TokenStream stream = newTokenStream(&pool, &(istream.stream));
    assertEquals(string, "GH.J", stream.next(&stream).str);
    assertEquals(string, "->", stream.next(&stream).str);
    assertEquals(string, "u.u.r", stream.next(&stream).str);
    assertEquals(string, "", stream.next(&stream).str);
  }

  pool.drain(&pool);
}
//-----------------------------------------------------------------------------
static void testQuotedString()
{
  AutoReleasePool pool = newAutoReleasePool();

  {
    InMemoryCharStream istream = newInMemoryCharStream("B \"3 & ^ 5\" xp ");
    TokenStream stream = newTokenStream(&pool, &(istream.stream));
    assertEquals(string, "B", stream.next(&stream).str);
    assertEquals(string, "\"3 & ^ 5\"", stream.next(&stream).str);
    assertEquals(string, "xp", stream.next(&stream).str);
  }
  {
    InMemoryCharStream istream = newInMemoryCharStream("B '3 $%5' xp ");
    TokenStream stream = newTokenStream(&pool, &(istream.stream));
    assertEquals(string, "B", stream.next(&stream).str);
    assertEquals(string, "'3 $%5'", stream.next(&stream).str);
    assertEquals(string, "xp", stream.next(&stream).str);
  }
  {
    InMemoryCharStream istream = newInMemoryCharStream(" ARYr `3 $%5` Xp ");
    TokenStream stream = newTokenStream(&pool, &(istream.stream));
    assertEquals(string, "ARYr", stream.next(&stream).str);
    assertEquals(string, "`3 $%5`", stream.next(&stream).str);
    assertEquals(string, "Xp", stream.next(&stream).str);
  }
  {
    InMemoryCharStream istream = newInMemoryCharStream("\"3 & ^ 5\" xp ");
    TokenStream stream = newTokenStream(&pool, &(istream.stream));
    assertEquals(string, "\"3 & ^ 5\"", stream.next(&stream).str);
    assertEquals(string, "xp", stream.next(&stream).str);
  }
  {
    InMemoryCharStream istream = newInMemoryCharStream("B \"3 & ^ 5\"");
    TokenStream stream = newTokenStream(&pool, &(istream.stream));
    assertEquals(string, "B", stream.next(&stream).str);
    assertEquals(string, "\"3 & ^ 5\"", stream.next(&stream).str);
  }
  {
    InMemoryCharStream istream = newInMemoryCharStream("B \"3 & ^ 5");
    TokenStream stream = newTokenStream(&pool, &(istream.stream));
    assertEquals(string, "B", stream.next(&stream).str);
    assertEquals(string, "\"3 & ^ 5", stream.next(&stream).str);
  }
  {
    InMemoryCharStream istream = newInMemoryCharStream("B \"3 & ^ \\\"5\\\" \" xp ");
    TokenStream stream = newTokenStream(&pool, &(istream.stream));
    assertEquals(string, "B", stream.next(&stream).str);
    assertEquals(string, "\"3 & ^ \\\"5\\\" \"", stream.next(&stream).str);
    assertEquals(string, "xp", stream.next(&stream).str);
  }
  {
    InMemoryCharStream istream = newInMemoryCharStream("B \"3 & ^ \\\"5\\\" \\\\\" xp ");
    TokenStream stream = newTokenStream(&pool, &(istream.stream));
    assertEquals(string, "B", stream.next(&stream).str);
    assertEquals(string, "\"3 & ^ \\\"5\\\" \\\\\"", stream.next(&stream).str);
    assertEquals(string, "xp", stream.next(&stream).str);
  }

  pool.drain(&pool);
}
//-----------------------------------------------------------------------------