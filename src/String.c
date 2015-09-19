//=============================================================================
// String.c
//=============================================================================
#include "String.h"
//-----------------------------------------------------------------------------
String StringOf(const string s)
{
  String str = {
    .str = s,
    .size = strlen(s)
  };

  return str;
}
//-----------------------------------------------------------------------------
string newstring(AutoReleasePool* const pool, const unsigned long size)
{
  return pool->calloc(pool, size, sizeof(CHAR));
}
//-----------------------------------------------------------------------------
String newString(AutoReleasePool* const pool, const unsigned long size)
{
  String str = {
    .str = newstring(pool, size + 1),
    .size = size
  };

  return str;
}
//-----------------------------------------------------------------------------
string copystring(AutoReleasePool* const pool, const string str, const unsigned long size)
{
  const unsigned long len = strlen(str);
  return strncpy(newstring(pool, size), str, len >= size? size - 1 : len);
}
//-----------------------------------------------------------------------------
String copyString(AutoReleasePool* const pool, const String str, const unsigned long size)
{
  String copy = {
    .str = copystring(pool, str.str, size + 1),
    .size = size
  };

  return copy;
}
//-----------------------------------------------------------------------------
String appendChar(AutoReleasePool* const pool, const String s, const CHAR c, const unsigned int increaseBy)
{
  const unsigned long used = strlen(s.str);

  if(used < s.size)
  {
    s.str[used] = c;
    return s;
  }

  String copy = copyString(pool, s, s.size + increaseBy);
  copy.str[used] = c;
  return copy;
}
//-----------------------------------------------------------------------------
string joinstrings(AutoReleasePool* const pool, const string a, const string b)
{
  unsigned int alen = strlen(a);
  string copy = copystring(pool, a, alen + strlen(b) + 1);
  strcpy(copy + alen, b);
  return copy;
}
//-----------------------------------------------------------------------------
String joinStrings(AutoReleasePool* const pool, const String a, const String b)
{
  string joined = joinstrings(pool, a.str, b.str);

  String copy = {
    .str = joined,
    .size = strlen(joined)
  };

  return copy;
}
//-----------------------------------------------------------------------------
String trimStringToSize(AutoReleasePool* const pool, const String s)
{
  const unsigned long used = strlen(s.str);
  return s.size > used? copyString(pool, s, used) : s;
}
//-----------------------------------------------------------------------------