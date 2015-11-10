//=============================================================================
// BracketStream.h
//=============================================================================
#ifndef SRC_BRACKETSTREAM_H_
#define SRC_BRACKETSTREAM_H_

#include "CharStream.h"
//-----------------------------------------------------------------------------
typedef enum BracketTokenType {
  curlyBrace,
  squareBracket,
  parentheses,
  statement,
  fileBracket,
  doubleQuote,
  singleQuote,
  backtick,
  singleLineComment,
  multiLineComment,
  token,
  unknown
} BracketTokenType;
//-----------------------------------------------------------------------------
extern const string bracketStreamBeginBrackets;
extern const string bracketStreamEndBrackets;
extern const string bracketStreamQuoteChars;
//-----------------------------------------------------------------------------
STRUCT {
  unsigned int line;
  unsigned int linePosition;
} StreamPosition;
//-----------------------------------------------------------------------------
STRUCT {
  CHAR c;
  BracketTokenType type;
  StreamPosition position;
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