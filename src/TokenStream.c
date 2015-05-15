#include "TokenStream.h"

#define NO_CHAR 0
#define IS_EMPTY(S) (S.str[0] == NO_CHAR)

static String next(TokenStream* stream);
static String getQuotedString(TokenStream* stream, character quoteChar, String buffer);

TokenStream getTokenStream(CharStream* const charStream)
{
  TokenStream tokenStream = {
    .next = *next,
    .charStream = charStream,
    .lastChar = NO_CHAR
  };

  return tokenStream;
}

static int containsChar(const character c, const character* const list)
{
  for(const character* i = list; *i != 0; i++)
    if(c == *i)
      return 1;

  return 0;
}

#define NEXTC NEXT(stream->charStream)

static const unsigned long bufferSize = 256;

static String next(TokenStream* const stream)
{
  static character* whitespace = " \n\r\t";
  String buffer = newString(bufferSize);

  character c;

  for(c = stream->lastChar == NO_CHAR? NEXTC : stream->lastChar; containsChar(c, whitespace); c = NEXTC);
  
  stream->lastChar = NO_CHAR;
  
  if(containsChar(c, "\"'`"))
    return getQuotedString(stream, c, buffer);

  for(;c != END_STREAM; c = NEXTC)
    if(c >= 'a' && c <= 'z')
      buffer = appendChar(buffer, c, bufferSize);
    else if(c >= 'A' && c <= 'Z')
      buffer = appendChar(buffer, c, bufferSize);
    else if(c =='_')
      buffer = appendChar(buffer, c, bufferSize);
    else
      break;

  if(IS_EMPTY(buffer))
    return trimString(appendChar(buffer, c, 0));

  stream->lastChar = c;
  return trimString(buffer);
}

static String getQuotedString(TokenStream* const stream, const character quoteChar, String buffer)
{
  buffer = appendChar(buffer, quoteChar, bufferSize);
  
  for(character c = NEXTC; c != END_STREAM; c = NEXTC)
  {
    buffer = appendChar(buffer, c, bufferSize);
    
    if(c == quoteChar)
      break;
  }
  
  return trimString(buffer);
}