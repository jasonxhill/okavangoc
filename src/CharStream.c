#include "CharStream.h"

static character next(CharStream* const stream)
{
  InMemoryCharStream* const istream = (InMemoryCharStream*) stream;
  const character c = istream->chars[0];

  if(c == END_STREAM)
    return END_STREAM;

  istream->chars++;
  return c;
}

InMemoryCharStream getInMemoryCharStream(const character* const chars)
{
  InMemoryCharStream stream = {
    .stream = {.next = *next},
    .chars = chars
  };

  return stream;
}
