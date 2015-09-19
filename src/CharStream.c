//=============================================================================
// CharStream.c
//=============================================================================
#include "CharStream.h"
//-----------------------------------------------------------------------------
static CHAR next(CharStream* const stream)
{
  InMemoryCharStream* const istream = (InMemoryCharStream*) stream;
  const CHAR c = istream->chars[0];

  if(c == END_STREAM)
    return END_STREAM;

  istream->chars++;
  return c;
}
//-----------------------------------------------------------------------------
InMemoryCharStream newInMemoryCharStream(const string str)
{
  InMemoryCharStream stream = {
    .stream = {.next = *next},
    .chars = str
  };

  return stream;
}
//-----------------------------------------------------------------------------