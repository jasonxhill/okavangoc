//=============================================================================
// TestUtil.h
//=============================================================================
#ifndef TEST_TESTUTIL_H_
#define TEST_TESTUTIL_H_

#include <stdio.h>
#include "../src/String.h"
//-----------------------------------------------------------------------------
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
//-----------------------------------------------------------------------------
typedef unsigned int unsigned_int;
//-----------------------------------------------------------------------------
#define TEST(METHOD) printf("[Running Test %s]\n", #METHOD); fflush(stdout); METHOD()
//-----------------------------------------------------------------------------
#define DECLARE_ASSERT(TYPE) void _assertEquals_##TYPE(const TYPE a, const TYPE b, const string file, const unsigned int line)
//-----------------------------------------------------------------------------
DECLARE_ASSERT(int);
DECLARE_ASSERT(unsigned_int);
DECLARE_ASSERT(char);
DECLARE_ASSERT(string);
//-----------------------------------------------------------------------------
#define assertEquals(TYPE, A, B) _assertEquals_##TYPE(A, B, __FILE__, __LINE__)
//-----------------------------------------------------------------------------
#endif // TEST_TESTUTIL_H_
//-----------------------------------------------------------------------------