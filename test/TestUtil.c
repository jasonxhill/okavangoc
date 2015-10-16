//=============================================================================
// TestUtil.c
//=============================================================================
#include "TestUtil.h"
//-----------------------------------------------------------------------------
#define DEFINE_ASSERT(TYPE, EXPR, SPECIFIER) \
DECLARE_ASSERT(TYPE) \
{\
  if(EXPR) {\
    printf(ANSI_COLOR_RED "[assertEquals] expected [" #SPECIFIER "] actual [" #SPECIFIER "] @ file: %s line: %i\n" ANSI_COLOR_RESET, a, b, file, line);\
    exit(EXIT_FAILURE);\
  }\
}
//-----------------------------------------------------------------------------
DEFINE_ASSERT(int, a != b, %i)
DEFINE_ASSERT(unsigned_int, a != b, %i)
DEFINE_ASSERT(char, a != b, %c)
DEFINE_ASSERT(string, (strcmp(a, b) != 0), %s)
//-----------------------------------------------------------------------------