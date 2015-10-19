//=============================================================================
// BracketStream.h
//=============================================================================
#ifndef SRC_BRACKETSTREAM_H_
#define SRC_BRACKETSTREAM_H_

#include "CharStream.h"
//-----------------------------------------------------------------------------
STRUCT {
  CHAR c;
  unsigned int line;
  unsigned int linePosition;
} StreamChar;
//-----------------------------------------------------------------------------
STRUCT BracketVisitor {
  void (* const visitBracketStart)(struct BracketVisitor*, StreamChar type);
  void (* const visitBracketEnd)(struct BracketVisitor*, StreamChar type);
  void (* const visitBracketEndMissing)(struct BracketVisitor*, StreamChar type);
  void (* const visitChar)(struct BracketVisitor*, StreamChar c);
} BracketVisitor;
//-----------------------------------------------------------------------------
STRUCT BracketStream {
  void (*visit)(struct BracketStream*, BracketVisitor*);
  CharStream* charStream;
} BracketStream;
//-----------------------------------------------------------------------------
BracketStream newBracketStream(CharStream*);
//-----------------------------------------------------------------------------
#endif // SRC_BRACKETSTREAM_H_
//-----------------------------------------------------------------------------