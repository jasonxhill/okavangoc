//=============================================================================
// BracketStream.h
//=============================================================================
#ifndef SRC_BRACKETSTREAM_H_
#define SRC_BRACKETSTREAM_H_

#include "CharStream.h"
#include "String.h"
//-----------------------------------------------------------------------------
STRUCT BracketVisitor {
  void (* const visitBracketStart)(struct BracketVisitor*, CHAR type);
  void (* const visitBracketEnd)(struct BracketVisitor*, CHAR type);
  void (* const visitString)(struct BracketVisitor*, String);
} BracketVisitor;
//-----------------------------------------------------------------------------
STRUCT BracketStream {
  void (*visit)(struct BracketStream*, BracketVisitor*);
  AutoReleasePool pool;
  CharStream* charStream;
} BracketStream;
//-----------------------------------------------------------------------------
BracketStream newBracketStream(CharStream*);
//-----------------------------------------------------------------------------
#endif // SRC_BRACKETSTREAM_H_
//-----------------------------------------------------------------------------