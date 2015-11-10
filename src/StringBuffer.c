//=============================================================================
// StringBuffer.c
//=============================================================================
#include "StringBuffer.h"
//-----------------------------------------------------------------------------
static const unsigned int increaseSizeBy = 31;
static StringBuffer appendStringToBuffer(StringBuffer*, string);
static StringBuffer appendCharToBuffer(StringBuffer*, CHAR);
static string stringBufferToString(StringBuffer*, MemoryPool*);
//-----------------------------------------------------------------------------
StringBuffer newStringBuffer(MemoryPool* const pool, const string s)
{
  const unsigned int len = strlen(s);

  StringBuffer sb = {
    .str = copyString(pool, s, len > increaseSizeBy? len : increaseSizeBy),
    .size = increaseSizeBy,
    .append = &appendStringToBuffer,
    .appendChar = &appendCharToBuffer,
    .toString = &stringBufferToString,
    .memoryPool = pool
  };

  return sb;
}
//-----------------------------------------------------------------------------
static StringBuffer appendStringToBuffer(StringBuffer* const buffer, const string s)
{
  const unsigned int used = strlen(buffer->str);
  unsigned int nextSize = buffer->size;
  for(const unsigned int requiredSize = used + strlen(s); nextSize < requiredSize; nextSize += increaseSizeBy);

  if(nextSize > buffer->size)
  {
    buffer->size = nextSize;
    buffer->str = copyString(buffer->memoryPool, buffer->str, nextSize);
  }

  strcpy(buffer->str + used, s);
  return *buffer;
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
  return duplicateString(memoryPool, buffer->str);
}
//-----------------------------------------------------------------------------