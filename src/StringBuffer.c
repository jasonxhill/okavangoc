//=============================================================================
// StringBuffer.c
//=============================================================================
#include "StringBuffer.h"
//-----------------------------------------------------------------------------
static const unsigned int increaseSizeBy = 31;
static StringBuffer appendCharToBuffer(StringBuffer*, CHAR);
static string stringBufferToString(StringBuffer*, MemoryPool*);
//-----------------------------------------------------------------------------
StringBuffer newStringBuffer(MemoryPool* const pool, const string s)
{
  const unsigned int len = strlen(s);

  StringBuffer sb = {
    .str = copyString(pool, s, len > increaseSizeBy? len : increaseSizeBy),
    .size = increaseSizeBy,
    .appendChar = &appendCharToBuffer,
    .toString = &stringBufferToString,
    .memoryPool = pool
  };

  return sb;
}
//-----------------------------------------------------------------------------
static StringBuffer appendCharToBuffer(StringBuffer* const buffer, const CHAR c)
{
  const unsigned long used = strlen(buffer->str);

  if(used + 1 < buffer->size)
    buffer->str[used] = c;
  else
  {
    buffer->size += increaseSizeBy;
    buffer->str = copyString(buffer->memoryPool, buffer->str, buffer->size);
    buffer->str[used] = c;
  }

  return *buffer;
}
//-----------------------------------------------------------------------------
static string stringBufferToString(StringBuffer* const buffer, MemoryPool* const memoryPool)
{
  return copyString(memoryPool, buffer->str, strlen(buffer->str));
}
//-----------------------------------------------------------------------------