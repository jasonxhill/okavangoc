//=============================================================================
// BracketStream.c
//=============================================================================
#include "BracketStream.h"
//-----------------------------------------------------------------------------
static void visit(BracketStream*, BracketVisitor*);
static void visitBracket(BracketStream*, BracketVisitor*, CHAR, CHAR);
//-----------------------------------------------------------------------------
BracketStream newBracketStream(CharStream* const c)
{
  BracketStream bracketStream = {
    .visit = &visit,
    .pool = newAutoReleasePool(),
    .charStream = c
  };

  return bracketStream;
}
//-----------------------------------------------------------------------------
static void visit(struct BracketStream* const stream, BracketVisitor* const visitor)
{
  visitor->visitBracketStart(visitor, END_STREAM);

  for(CHAR c = NEXT(stream->charStream); c != END_STREAM; c = NEXT(stream->charStream))
    visitBracket(stream, visitor, ';', c);

  visitor->visitBracketEnd(visitor, END_STREAM);
}
//-----------------------------------------------------------------------------
static void visitBracket(BracketStream* const stream, BracketVisitor* const visitor, const CHAR type, const CHAR currentChar)
{
  visitor->visitBracketStart(visitor, type);

  for(CHAR c = currentChar; c != END_STREAM; c = NEXT(stream->charStream))
  {
    if(c == type)
    {
      visitor->visitBracketEnd(visitor, type);
      return;
    }

    int pos = charstringPos(c, 0, "{[(");

    if(pos >= 0)
    {
      CHAR nextC = NEXT(stream->charStream);
      visitBracket(stream, visitor, "}])"[pos], nextC);
    }
  }
}
//-----------------------------------------------------------------------------