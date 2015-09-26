//=============================================================================
// BracketStream.c
//=============================================================================
#include "BracketStream.h"
//-----------------------------------------------------------------------------
static void visit(BracketStream*, BracketVisitor*);
static CHAR visitBracket(BracketStream*, BracketVisitor*, CHAR, CHAR, CHAR);
static void visitQuotedToken(BracketStream*, BracketVisitor*, CHAR, CHAR, CHAR, CHAR);
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
  visitBracket(stream, visitor, END_STREAM, END_STREAM, NEXT(stream->charStream));
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
                         const CHAR endChar,
                         CHAR c)
{
  const string beginBrackets = "{[(";
  const string endBrackets = "}])";
  const string quoteChars = "\"'`";

  VISIT_START(type)
  BOOL startStatementNeeded = TRUE;
  BOOL commentIndicator = FALSE;
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  for(; c != END_STREAM || endChar == END_STREAM; c = NEXT(stream->charStream))
  {
    //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    // Check for end bracket
    if(c == endChar)
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
    // Check for line comments
    if(commentIndicator)
    {
      commentIndicator = FALSE;

      if(c == '/')
      {
        visitQuotedToken(stream, visitor, '/', '\n', '/', '/');
        continue;
      }
      else
        VISIT_CHAR('/')
    }
    else if(c == '/')
    {
      commentIndicator = TRUE;
      continue;
    }
    //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    // Check for quotes
    if(containsChar(c, quoteChars))
    {
      visitQuotedToken(stream, visitor, c, c, c, '\0');
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
    c = visitBracket(stream, visitor, c, endBracket, NEXT(stream->charStream));

    if(c == NO_CHAR)
      continue;
    //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    // The nested statement terminated incorrectly
    END_STATEMENT

    if(c == endChar)
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
static void visitQuotedToken(BracketStream* const stream,
                             BracketVisitor* const visitor,
                             const CHAR type,
                             const CHAR endChar,
                             const CHAR c0,
                             const CHAR c1)
{
  VISIT_START(type)
  VISIT_CHAR(c0)

//  if(c1 != '\0')
    VISIT_CHAR(c1)

  BOOL escaped = FALSE;

  for(CHAR c = NEXT(stream->charStream); c != END_STREAM; c = NEXT(stream->charStream))
  {
    VISIT_CHAR(c)

    if(c == '\\')
    {
      escaped = !escaped;
      continue;
    }
    if(c == endChar && !escaped)
    {
      VISIT_END(type)
      return;
    }

    escaped = FALSE;
  }

  VISIT_MISSING(type)
}
//-----------------------------------------------------------------------------