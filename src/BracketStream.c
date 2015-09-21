//=============================================================================
// BracketStream.c
//=============================================================================
#include "BracketStream.h"
//-----------------------------------------------------------------------------
static void visit(BracketStream*, BracketVisitor*);
static CHAR visitBracket(BracketStream*, BracketVisitor*, CHAR, CHAR);
static void visitQuotedString(BracketStream*, BracketVisitor*, CHAR quoteChar);
//-----------------------------------------------------------------------------
BracketStream newBracketStream(CharStream* const c)
{
  BracketStream bracketStream = {
    .visit = &visit,
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
#define VISIT_START(TYPE) visitor->visitBracketStart(visitor, TYPE);
#define VISIT_END(TYPE) visitor->visitBracketEnd(visitor, TYPE);
#define VISIT_MISSING(TYPE) visitor->visitBracketEndMissing(visitor, TYPE);
#define VISIT_CHAR(C) visitor->visitChar(visitor, C);
//-----------------------------------------------------------------------------
#define END_STATEMENT if(!startStatementNeeded)                       \
                      {                                               \
                        VISIT_MISSING(';')\
                      }
//-----------------------------------------------------------------------------
static CHAR visitBracket(BracketStream* const stream,
                         BracketVisitor* const visitor,
                         const CHAR type,
                         CHAR c)
{
  const string beginBrackets = "{[(";
  const string endBrackets = "}])";
  const string quoteChars = "\"'`";

  VISIT_START(type)
  BOOL startStatementNeeded = TRUE;
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  for(; c != END_STREAM || type == END_STREAM; c = NEXT(stream->charStream))
  {
    //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    // Check for end bracket
    if(c == type)
    {
      END_STATEMENT
      VISIT_END(type)
      return NO_CHAR;
    }
    if(containsChar(c, endBrackets))
    {
      END_STATEMENT
      VISIT_MISSING(type)
      return c;
    }
    //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    // Check start and end of statements
    if(startStatementNeeded)
    {
      startStatementNeeded = FALSE;
      VISIT_START(';')
    }
    if(c == ';')
    {
      startStatementNeeded = TRUE;
      VISIT_END(';')
      continue;
    }
    //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    // Check for quotes
    if(containsChar(c, quoteChars))
    {
      visitQuotedString(stream, visitor, c);
      continue;
    }
    //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    // Check for start of nested bracket
    if(!containsChar(c, beginBrackets))
    {
      VISIT_CHAR(c)
      continue;
    }
    //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    // Start a nested bracket
    const CHAR endBracket = endBrackets[charstringPos(c, 0, beginBrackets)];
    c = NEXT(stream->charStream);
    c = visitBracket(stream, visitor, endBracket, c);

    if(c == NO_CHAR)
      continue;
    //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    // The nested statement terminated incorrectly
    END_STATEMENT

    if(c == type)
    {
      VISIT_END(type)
      return NO_CHAR;
    }

    VISIT_MISSING(type)
    return c;
    //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
  }
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
  // No closing bracket was found
  END_STATEMENT
  VISIT_MISSING(type)
  return NO_CHAR;
  //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
}
//-----------------------------------------------------------------------------
static void visitQuotedString(BracketStream* const stream, BracketVisitor* const visitor, const CHAR quoteChar)
{
  VISIT_START(quoteChar)
  VISIT_CHAR(quoteChar)
  BOOL escaped = FALSE;

  for(CHAR c = NEXT(stream->charStream); c != END_STREAM; c = NEXT(stream->charStream))
  {
    VISIT_CHAR(c)

    if(c == '\\')
    {
      escaped = !escaped;
      continue;
    }
    if(c == quoteChar && !escaped)
    {
      VISIT_END(quoteChar)
      return;
    }

    escaped = FALSE;
  }

  VISIT_MISSING(quoteChar)
}
//-----------------------------------------------------------------------------