//=============================================================================
// String.h
//=============================================================================
#ifndef SRC_STRING_H_
#define SRC_STRING_H_

#include "AutoReleasePool.h"
#include <string.h>
//-----------------------------------------------------------------------------
STRUCT String {
  const string str;
  const unsigned long size;
} String;
//-----------------------------------------------------------------------------
String StringOf(const string);
string newstring(AutoReleasePool*, unsigned long size);
String newString(AutoReleasePool*, unsigned long size);
string copystring(AutoReleasePool*, const string, unsigned long size);
String copyString(AutoReleasePool*, const String, unsigned long size);
String appendChar(AutoReleasePool*, const String, const CHAR, unsigned int increaseBy);
string joinstrings(AutoReleasePool*, const string, const string);
String joinStrings(AutoReleasePool*, const String, const String);
String trimStringToSize(AutoReleasePool*, const String);
unsigned int getAllocationCount();
//-----------------------------------------------------------------------------
#endif // SRC_STRING_H_
//-----------------------------------------------------------------------------