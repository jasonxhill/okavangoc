//=============================================================================
// String.c
//=============================================================================
#include "String.h"
//-----------------------------------------------------------------------------
string newString(MemoryPool* const pool, const unsigned long size)
{
  return pool->calloc(pool, size + 1, sizeof(CHAR));
}
//-----------------------------------------------------------------------------
string copyString(MemoryPool* const pool, const string str, const unsigned long size)
{
  const unsigned long len = strlen(str);
  return strncpy(newString(pool, size), str, len >= size? size : len);
}
//-----------------------------------------------------------------------------
string joinStrings(MemoryPool* const pool, const string a, const string b)
{
  unsigned int alen = strlen(a);
  string copy = copyString(pool, a, alen + strlen(b));
  strcpy(copy + alen, b);
  return copy;
}
//-----------------------------------------------------------------------------
BOOL containsChar(const CHAR c, const string s)
{
  return charStringPos(c, 0, s) >= 0;
}
//-----------------------------------------------------------------------------
int charStringPos(const CHAR c, unsigned int start, const string s)
{
  for(int i = start; s[i] != 0; i++)
    if(c == s[i])
      return i;

  return -1;
}
//-----------------------------------------------------------------------------
unsigned long hashString(const string str)
{
  unsigned long hash = 5381;
  CHAR* s = str;
  int c;

  while((c = *s++))
    hash = ((hash << 5) + hash) + c;

  return hash;
}
//-----------------------------------------------------------------------------