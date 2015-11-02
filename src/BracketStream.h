//=============================================================================
// BracketStream.h
//=============================================================================
#ifndef SRC_BRACKETSTREAM_H_
#define SRC_BRACKETSTREAM_H_

#include "CharStream.h"
//-----------------------------------------------------------------------------
extern const string bracketStreamBeginBrackets;
extern const string bracketStreamEndBrackets;
extern const string bracketStreamQuoteChars;
//-----------------------------------------------------------------------------
STRUCT {
  CHAR c;
  unsigned int line;
  unsigned int linePosition;
} StreamChar;
//-----------------------------------------------------------------------------
struct BracketVisitor;
typedef void (* BracketVisitStart)(struct BracketVisitor*, StreamChar);
typedef void (* BracketVisitEnd)(struct BracketVisitor*, StreamChar);
typedef void (* BracketVisitEndMissing)(struct BracketVisitor*, StreamChar);
typedef void (* BracketVisitChar)(struct BracketVisitor*, StreamChar);
//-----------------------------------------------------------------------------
STRUCT BracketVisitor {
  BracketVisitStart visitBracketStart;
  BracketVisitEnd visitBracketEnd;
  BracketVisitEndMissing visitBracketEndMissing;
  BracketVisitChar visitChar;
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