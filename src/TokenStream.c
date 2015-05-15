#include "TokenStream.h"

#define NO_CHAR 0
#define IS_EMPTY(S) (S.str[0] == NO_CHAR)

static String next(TokenStream* const stream);

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

static String next(TokenStream* const stream)
{
  static character* whitespace = " \n\r\t";
  static const unsigned int bufferSize = 256;
  String buffer = newString(bufferSize);

  character c;

  for(c = stream->lastChar == NO_CHAR? NEXTC : stream->lastChar; containsChar(c, whitespace); c = NEXTC);

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
  {
    stream->lastChar = NO_CHAR;
    return trimString(appendChar(buffer, c, 0));
  }

  stream->lastChar = c;
  return trimString(buffer);
}
