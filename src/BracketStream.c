//=============================================================================
// BracketStream.c
//=============================================================================
#include "BracketStream.h"
//-----------------------------------------------------------------------------
static void visit(BracketStream*, BracketVisitor*);
static CHAR visitBracket(BracketStream*, BracketVisitor*, CHAR, CHAR);
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
  visitBracket(stream, visitor, END_STREAM, NEXT(stream->charStream));
}
//-----------------------------------------------------------------------------
#define END_STATEMENT if(!startStatementNeeded) \
                        visitor->visitBracketEndMissing(visitor, ';');

static CHAR visitBracket(BracketStream* const stream, BracketVisitor* const visitor, const CHAR type, CHAR c)
{
  visitor->visitBracketStart(visitor, type);

  BOOL startStatementNeeded = TRUE;

  for(; c != END_STREAM || type == END_STREAM; c = NEXT(stream->charStream))
  {
    if(c == type)
    {
      END_STATEMENT
      visitor->visitBracketEnd(visitor, type);
      return NO_CHAR;
    }

    int pos = charstringPos(c, 0, "}])");

    if(pos >= 0)
    {
      END_STATEMENT
      visitor->visitBracketEndMissing(visitor, type);
      return c;
    }

    if(startStatementNeeded)
    {
      startStatementNeeded = FALSE;
      visitor->visitBracketStart(visitor, ';');
    }
    if(c == ';')
    {
      startStatementNeeded = TRUE;
      visitor->visitBracketEnd(visitor, ';');
      continue;
    }

    pos = charstringPos(c, 0, "{[(");

    if(pos < 0)
      continue;

    c = NEXT(stream->charStream);
    c = visitBracket(stream, visitor, "}])"[pos], c);

    if(c == NO_CHAR)
      continue;

    END_STATEMENT

    if(c == type)
    {
      visitor->visitBracketEnd(visitor, type);
      return NO_CHAR;
    }

    visitor->visitBracketEndMissing(visitor, type);
    return c;
  }

  END_STATEMENT
  visitor->visitBracketEndMissing(visitor, type);
  return NO_CHAR;
}
//-----------------------------------------------------------------------------