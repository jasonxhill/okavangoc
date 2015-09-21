//=============================================================================
// BracketStream.c
//=============================================================================
#include "BracketStream.h"
//-----------------------------------------------------------------------------
static void visit(BracketStream*, BracketVisitor*);
static CHAR visitBracket(BracketStream*, BracketVisitor*, CHAR, CHAR);
static CHAR visitBracketWithPool(BracketStream* const stream,
                                 BracketVisitor* const visitor,
                                 const CHAR type,
                                 CHAR c,
                                 AutoReleasePool* const pool);
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
static CHAR visitBracket(BracketStream* const stream, BracketVisitor* const visitor, const CHAR type, CHAR c)
{
  AutoReleasePool pool = newAutoReleasePool();
  c = visitBracketWithPool(stream, visitor, type, c, &pool);
  pool.drain(&pool);
  return c;
}
//-----------------------------------------------------------------------------
#define VISIT_END(TYPE) visitor->visitBracketEnd(visitor, TYPE);
#define VISIT_MISSING(TYPE) visitor->visitBracketEndMissing(visitor, TYPE);

#define VISIT_STRING if(value.size > 0)                     \
                     {                                      \
                       visitor->visitString(visitor, value);\
                       value = StringOf("");                \
                     }

#define END_STATEMENT if(!startStatementNeeded)                       \
                      {                                               \
                        VISIT_STRING                                  \
                        VISIT_MISSING(';');\
                      }

static CHAR visitBracketWithPool(BracketStream* const stream,
                                 BracketVisitor* const visitor,
                                 const CHAR type,
                                 CHAR c,
                                 AutoReleasePool* const pool)
{
  visitor->visitBracketStart(visitor, type);

  BOOL startStatementNeeded = TRUE;
  String value = StringOf("");

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
    if(charstringPos(c, 0, "}])0") >= 0)
    {
      END_STATEMENT
      VISIT_MISSING(type)
      return c;
    }
    //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    // Check start and end of statments
    if(startStatementNeeded)
    {
      startStatementNeeded = FALSE;
      visitor->visitBracketStart(visitor, ';');
    }
    if(c == ';')
    {
      startStatementNeeded = TRUE;
      VISIT_STRING
      VISIT_END(';');
      continue;
    }
    //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    // Check for start of nested bracket
    const int pos = charstringPos(c, 0, "{[(");

    if(pos < 0)
    {
      value = appendChar(pool, value, c, 255);
      continue;
    }
    //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    // Start a nested bracket
    VISIT_STRING
    c = NEXT(stream->charStream);
    c = visitBracket(stream, visitor, "}])"[pos], c);

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