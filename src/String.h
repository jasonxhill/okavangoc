//=============================================================================
// String.h
//=============================================================================
#ifndef SRC_STRING_H_
#define SRC_STRING_H_

#include "MemoryPool.h"
#include <string.h>
//-----------------------------------------------------------------------------
STRUCT String {
  string str;
  unsigned long size;
} String;
//-----------------------------------------------------------------------------
String StringOf(const string);
string newstring(MemoryPool*, unsigned long size);
String newString(MemoryPool*, unsigned long size);
string copystring(MemoryPool*, const string, unsigned long size);
String copyString(MemoryPool*, const String, unsigned long size);
String appendChar(MemoryPool*, const String, const CHAR, unsigned int increaseBy);
string joinstrings(MemoryPool*, const string, const string);
String joinStrings(MemoryPool*, const String, const String);
String trimStringToSize(MemoryPool*, const String);
BOOL containsChar(CHAR, const string);
int charstringPos(CHAR, unsigned int start, const string);
unsigned long hashstring(const string);
//-----------------------------------------------------------------------------
#endif // SRC_STRING_H_
//-----------------------------------------------------------------------------