//=============================================================================
// TokenStream.h
//=============================================================================
#include "TokenStream.h"
//-----------------------------------------------------------------------------
#define NEXTC NEXT(stream->charStream)
//-----------------------------------------------------------------------------
static const unsigned long bufferSize = 256;
//-----------------------------------------------------------------------------
static String next(TokenStream*);
static String nextWithRelease(AutoReleasePool*, AutoReleasePool*, TokenStream*);
static String getQuotedString(AutoReleasePool*, TokenStream*, CHAR quoteChar, String buffer);
static BOOL isWhitespace(const CHAR);
static BOOL isAlpha(const CHAR);
static BOOL isNumber(const CHAR);
//-----------------------------------------------------------------------------
TokenStream newTokenStream(AutoReleasePool* pool, CharStream* const charStream)
{
  TokenStream tokenStream = {
    .next = *next,
    .autoReleasePool = pool,
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
  return trimStringToSize(parentPool, buffer);\
}
//-----------------------------------------------------------------------------
static String next(TokenStream* const stream)
{
  AutoReleasePool pool = newAutoReleasePool();
  String result = nextWithRelease(stream->autoReleasePool, &pool, stream);
  pool.drain(&pool);
  return result;
}
//-----------------------------------------------------------------------------
static String nextWithRelease(AutoReleasePool* const parentPool, AutoReleasePool* const pool,  TokenStream* const stream)
{
  String buffer = newString(pool, bufferSize);
  CHAR c;

  for(c = stream->lastChar == NO_CHAR? NEXTC : stream->lastChar; isWhitespace(c); c = NEXTC);

  stream->lastChar = NO_CHAR;

  if(containsChar(c, "\"'`"))
    return getQuotedString(parentPool, stream, c, buffer);

  for(;isAlpha(c) || isNumber(c) || containsChar(c, "._$"); c = NEXTC)
    buffer = appendChar(pool, buffer, c, bufferSize);

  CHECK_RESULTS

  for(;c != END_STREAM && !isWhitespace(c) && !isAlpha(c) && !isNumber(c); c = NEXTC)
    buffer = appendChar(pool, buffer, c, bufferSize);

  CHECK_RESULTS

  return trimStringToSize(parentPool, appendChar(pool, buffer, c, bufferSize));
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
static String getQuotedString(AutoReleasePool* const parentPool, TokenStream* const stream, const CHAR quoteChar, String buffer)
{
  AutoReleasePool pool = newAutoReleasePool();

  buffer = appendChar(&pool, buffer, quoteChar, bufferSize);
  BOOL escaped = FALSE;

  for(CHAR c = NEXTC; c != END_STREAM; c = NEXTC)
  {
    buffer = appendChar(&pool, buffer, c, bufferSize);

    if(c == '\\')
    {
      escaped = !escaped;
      continue;
    }

    if(c == quoteChar && !escaped)
      break;

    escaped = FALSE;
  }

  String result = trimStringToSize(parentPool, buffer);
  pool.drain(&pool);
  return result;
}
//-----------------------------------------------------------------------------