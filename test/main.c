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
extern void mainAutoReleasePoolTests();
//-----------------------------------------------------------------------------
int main(const int argc, const string argv[])
{
  mainStringTests();
  mainCharStreamTests();
  mainTokenStreamTests();
  mainBracketStreamTests();
  mainFileStreamTest();
  mainAutoReleasePoolTests();
  return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------