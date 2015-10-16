//=============================================================================
// FileStreamTest.c
//=============================================================================
#include "TestUtil.h"
#include "../src/CharStream.h"
#include "../src/StringBuffer.h"
//-----------------------------------------------------------------------------
static void testStreamFile();
//-----------------------------------------------------------------------------
void mainFileStreamTest()
{
  TEST(testStreamFile);
}
//-----------------------------------------------------------------------------
static void testStreamFile()
{
  MemoryPool pool = newMemoryPool();
  FileCharStream fstream = newFileCharStream("./test/TestMain.okv");
  CharStream* const stream = &(fstream.stream);
  StringBuffer s = newStringBuffer(&pool, "");

  CHAR c;

  while((c = stream->next(stream)) != END_STREAM)
    s.appendChar(&s, c);

  assertEquals(string, s.toString(&s, &pool),
    "export:\n"
    "{\n"
    "  main:import.out.puts(\"hello world!\");\n"
    "};"
  );

  fstream.close(&fstream);
  pool.drain(&pool);
}
//-----------------------------------------------------------------------------