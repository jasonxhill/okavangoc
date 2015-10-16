//=============================================================================
// main.c
//=============================================================================
#include "TestUtil.h"
//-----------------------------------------------------------------------------
extern void mainStringTests();
extern void mainCharStreamTests();
extern void mainTokenStreamTests();
extern void mainBracketStreamTests();
extern void mainFileStreamTest();
extern void mainLinkedListTests();
extern void mainAutoReleasePoolTests();
//-----------------------------------------------------------------------------
int main(const int argc, const string argv[])
{
  mainStringTests();
  mainCharStreamTests();
  mainTokenStreamTests();
  mainBracketStreamTests();
  mainFileStreamTest();
  mainLinkedListTests();
  mainAutoReleasePoolTests();
  return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------