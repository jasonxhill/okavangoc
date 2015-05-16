#include "TokenStream.h"

#define NO_CHAR 0
#define NEXTC NEXT(stream->charStream)

static String next(TokenStream* stream);
static String getQuotedString(TokenStream* stream, character quoteChar, String buffer);
static BOOL containsChar(const character c, const character* const list);
inline static BOOL isWhitespace(const character c);
inline static BOOL isAlpha(const character c);
inline static BOOL isNumber(const character c);

TokenStream getTokenStream(CharStream* const charStream)
{
  TokenStream tokenStream = {
    .next = *next,
    .charStream = charStream,
    .lastChar = NO_CHAR
  };

  return tokenStream;
}

static const unsigned long bufferSize = 256;

#define CHECK_RESULTS       \
if(buffer.str[0] != NO_CHAR)\
{                           \
  stream->lastChar = c;     \
  return trimString(buffer);\
}

static String next(TokenStream* const stream)
{
  String buffer = newString(bufferSize);
  character c;

  for(c = stream->lastChar == NO_CHAR? NEXTC : stream->lastChar; isWhitespace(c); c = NEXTC);
  stream->lastChar = NO_CHAR;

  if(containsChar(c, "\"'`"))
    return getQuotedString(stream, c, buffer);

  for(;isAlpha(c) || isNumber(c) || containsChar(c, "._$"); c = NEXTC)
    buffer = appendChar(buffer, c, bufferSize);

  CHECK_RESULTS
  
  for(;c != END_STREAM && !isWhitespace(c) && !isAlpha(c) && !isNumber(c); c = NEXTC)
    buffer = appendChar(buffer, c, bufferSize);
  
  CHECK_RESULTS
  
  return trimString(appendChar(buffer, c, 0));
}

inline static BOOL isWhitespace(const character c)
{
  return containsChar(c, " \n\r\t");
}

inline static BOOL isAlpha(const character c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

inline static BOOL isNumber(const character c)
{
  return c >= '0' && c <= '9';
}

static BOOL containsChar(const character c, const character* const list)
{
  for(const character* i = list; *i != 0; i++)
    if(c == *i)
      return TRUE;

  return FALSE;
}

static String getQuotedString(TokenStream* const stream, const character quoteChar, String buffer)
{
  buffer = appendChar(buffer, quoteChar, bufferSize);
  BOOL escaped = 0;

  for(character c = NEXTC; c != END_STREAM; c = NEXTC)
  {
    buffer = appendChar(buffer, c, bufferSize);

    if(c == '\\')
    {
      escaped = !escaped;
      continue;
    }

    if(c == quoteChar && escaped != TRUE)
      break;

    escaped = FALSE;
  }

  return trimString(buffer);
}
