//=============================================================================
// main.c
//=============================================================================
#include "TestUtil.h"
//-----------------------------------------------------------------------------
extern void mainStringTests();
extern void mainStringBufferTests();
extern void mainCharStreamTests();
extern void mainTokenStreamTests();
extern void mainBracketStreamTests();
extern void mainFileStreamTest();
extern void mainLinkedListTests();
extern void mainSyntaxTreeTests();
extern void mainMemoryPoolTests();
//-----------------------------------------------------------------------------
int main(const int argc, const string argv[])
{
  mainStringTests();
  mainStringBufferTests();
  mainCharStreamTests();
  mainTokenStreamTests();
  mainBracketStreamTests();
  mainFileStreamTest();
  mainLinkedListTests();
  mainSyntaxTreeTests();
  mainMemoryPoolTests();
  return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------