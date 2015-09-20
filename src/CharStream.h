//=============================================================================
// CharStream.h
//=============================================================================
#ifndef SRC_CHARSTREAM_H_
#define SRC_CHARSTREAM_H_
#include "Util.h"
//-----------------------------------------------------------------------------
#define END_STREAM 0
#define NO_CHAR 0
#define NEXT(STREAM) INVOKE(STREAM, next)
//-----------------------------------------------------------------------------
STRUCT CharStream {
  CHAR (*next)(struct CharStream*);
} CharStream;
//-----------------------------------------------------------------------------
STRUCT {
  CharStream stream;
  const CHAR* chars;
} InMemoryCharStream;
//-----------------------------------------------------------------------------
InMemoryCharStream newInMemoryCharStream(const string);
//-----------------------------------------------------------------------------
#endif // SRC_CHARSTREAM_H_
//-----------------------------------------------------------------------------