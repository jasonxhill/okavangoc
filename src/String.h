//=============================================================================
// String.h
//=============================================================================
#ifndef SRC_STRING_H_
#define SRC_STRING_H_

#include "MemoryPool.h"
#include <string.h>
//-----------------------------------------------------------------------------
string newString(MemoryPool*, unsigned long size);
string copyString(MemoryPool*, const string, unsigned long size);
string joinStrings(MemoryPool*, const string, const string);
BOOL containsChar(CHAR, const string);
int charStringPos(CHAR, unsigned int start, const string);
unsigned long hashString(const string);
//-----------------------------------------------------------------------------
#endif // SRC_STRING_H_
//-----------------------------------------------------------------------------