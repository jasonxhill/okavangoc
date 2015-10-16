//=============================================================================
// FileStreamTest.c
//=============================================================================
#include "TestUtil.h"
#include "../src/CharStream.h"
#include "../src/String.h"
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
  AutoReleasePool pool = newAutoReleasePool();
  FileCharStream fstream = newFileCharStream("./test/TestMain.okv");
  CharStream* const stream = &(fstream.stream);
  String s = StringOf("");

  CHAR c;

  while((c = stream->next(stream)) != END_STREAM)
    s = appendChar(&pool, s, c, 100);

  assertEquals(String, s, StringOf(
    "export:\n"
    "{\n"
    "  main:import.out.puts(\"hello world!\");\n"
    "};"
  ));

  fstream.close(&fstream);
  pool.drain(&pool);
}
//-----------------------------------------------------------------------------