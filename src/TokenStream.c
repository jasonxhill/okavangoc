//=============================================================================
// TokenStream.h
//=============================================================================
#include "TokenStream.h"
#include "StringBuffer.h"
//-----------------------------------------------------------------------------
#define NEXTC NEXT(stream->charStream)
//-----------------------------------------------------------------------------
static string next(TokenStream*);
static string nextWithRelease(MemoryPool*, MemoryPool*, TokenStream*);
static string getQuotedString(MemoryPool*, TokenStream*, CHAR quoteChar, StringBuffer);
static BOOL isWhitespace(const CHAR);
static BOOL isAlpha(const CHAR);
static BOOL isNumber(const CHAR);
//-----------------------------------------------------------------------------
TokenStream newTokenStream(MemoryPool* pool, CharStream* const charStream)
{
  TokenStream tokenStream = {
    .next = *next,
    .memoryPool = pool,
    .charStream = charStream,
    .lastChar = NO_CHAR
  };

  return tokenStream;
}
//-----------------------------------------------------------------------------
#define CHECK_RESULTS                         \
if(buffer.str[0] != NO_CHAR)                  \
{                                             \
  stream->lastChar = c;                       \
  return buffer.toString(&buffer, parentPool);\
}
//-----------------------------------------------------------------------------
static string next(TokenStream* const stream)
{
  MemoryPool pool = newMemoryPool();
  string result = nextWithRelease(stream->memoryPool, &pool, stream);
  pool.drain(&pool);
  return result;
}
//-----------------------------------------------------------------------------
static string nextWithRelease(MemoryPool* const parentPool, MemoryPool* const pool,  TokenStream* const stream)
{
  StringBuffer buffer = newStringBuffer(pool, "");
  CHAR c;

  for(c = stream->lastChar == NO_CHAR? NEXTC : stream->lastChar; isWhitespace(c); c = NEXTC);

  stream->lastChar = NO_CHAR;

  if(containsChar(c, "\"'`"))
    return getQuotedString(parentPool, stream, c, buffer);

  for(;isAlpha(c) || isNumber(c) || containsChar(c, "._$"); c = NEXTC)
    buffer.appendChar(&buffer, c);

  CHECK_RESULTS

  for(;c != END_STREAM && !isWhitespace(c) && !isAlpha(c) && !isNumber(c); c = NEXTC)
    buffer.appendChar(&buffer, c);

  CHECK_RESULTS

  buffer.appendChar(&buffer, c);
  return buffer.toString(&buffer, parentPool);
}
//-----------------------------------------------------------------------------
static BOOL isWhitespace(const CHAR c)
{
  return containsChar(c, " \n\r\t");
}
//-----------------------------------------------------------------------------
static BOOL isAlpha(const CHAR c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
//-----------------------------------------------------------------------------
static BOOL isNumber(const CHAR c)
{
  return c >= '0' && c <= '9';
}
//-----------------------------------------------------------------------------
static string getQuotedString(MemoryPool* const parentPool, TokenStream* const stream, const CHAR quoteChar, StringBuffer buffer)
{
  MemoryPool pool = newMemoryPool();

  buffer.appendChar(&buffer, quoteChar);
  BOOL escaped = FALSE;

  for(CHAR c = NEXTC; c != END_STREAM; c = NEXTC)
  {
    buffer.appendChar(&buffer, c);

    if(c == '\\')
    {
      escaped = !escaped;
      continue;
    }

    if(c == quoteChar && !escaped)
      break;

    escaped = FALSE;
  }

  string result = buffer.toString(&buffer, parentPool);
  pool.drain(&pool);
  return result;
}
//-----------------------------------------------------------------------------