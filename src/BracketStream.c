//=============================================================================
// BracketStream.c
//=============================================================================
#include "BracketStream.h"
#include "String.h"
//-----------------------------------------------------------------------------
const string bracketStreamBeginBrackets = "{[(";
const string bracketStreamEndBrackets = "}])";
const string bracketStreamQuoteChars = "\"'`";
//-----------------------------------------------------------------------------
static void visit(BracketStream*, BracketVisitor*);
static StreamChar visitBracket(BracketStream*, BracketVisitor*, StreamChar, CHAR, StreamChar);
static StreamChar visitQuotedToken(BracketStream*, BracketVisitor*, StreamChar);
static StreamChar visitSingleLineComment(BracketStream*, BracketVisitor*, StreamChar);
static StreamChar visitMultiLineComment(BracketStream*, BracketVisitor*, StreamChar);
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
static StreamChar replaceChar(StreamChar sc, const CHAR replacement)
{
  sc.c = replacement;
  return sc;
}
//-----------------------------------------------------------------------------
#define CASE(C,E) case C: sc.type = E; return sc;

static StreamChar setType(StreamChar sc)
{
  switch(sc.c)
  {
    CASE(';', statement)
    CASE('{', curlyBrace)
    CASE('[', squareBracket)
    CASE('(', parentheses)
    CASE(END_STREAM, fileBracket)
    CASE('"', doubleQuote)
    CASE('\'', singleQuote)
    CASE('`', backtick)
    CASE('/', singleLineComment)
    CASE('*', multiLineComment)
    default: sc.type = unknown; return sc;
  }
}
#undef CASE
//-----------------------------------------------------------------------------
static StreamChar nextChar(BracketStream* const stream, const StreamChar lastChar)
{
  const CHAR c = stream->charStream->next(stream->charStream);

  if(c == '\0')
    return replaceChar(lastChar, c);
  if(c != '\n') {
    const StreamChar sc = {.c = c, .type = token, .line = lastChar.line, .linePosition = lastChar.linePosition + 1};
    return sc;
  }

  const StreamChar sc = {.c = c, .type = token, .line = lastChar.line + 1, .linePosition = 0};
  return sc;
}
//-----------------------------------------------------------------------------
static void visit(BracketStream* const stream, BracketVisitor* const visitor)
{
  const StreamChar firstChar = {.c = END_STREAM, .type = token, .line = 1, .linePosition = 0};
  visitBracket(stream, visitor, firstChar, END_STREAM, nextChar(stream, firstChar));
}
//-----------------------------------------------------------------------------
#define SC(C) replaceChar(sc, C)
#define VISIT_START(VAR) visitor->visitBracketStart(visitor, setType(VAR));
#define VISIT_CHAR(VAR) visitor->visitChar(visitor, setType(VAR));
#define VISIT_END(VAR) visitor->visitBracketEnd(visitor, setType(SC(VAR)));
#define VISIT_MISSING(VAR) visitor->visitBracketEndMissing(visitor, setType(SC(VAR)));
//-----------------------------------------------------------------------------
#define END_STATEMENT if(!startStatementNeeded)\
                      {                        \
                        VISIT_MISSING(';')     \
                      }
//-----------------------------------------------------------------------------
static StreamChar visitBracket(BracketStream* const stream,
                               BracketVisitor* const visitor,
                               const StreamChar type,
                               const CHAR endChar,
                               StreamChar sc)
{
  VISIT_START(type)
  BOOL startStatementNeeded = TRUE;
  BOOL commentIndicator = FALSE;
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  for(; sc.c != END_STREAM || endChar == END_STREAM; sc = nextChar(stream, sc))
  {
    //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    // Check for end bracket
    if(sc.c == endChar)
    {
      END_STATEMENT
      VISIT_END(type.c)
      return SC(NO_CHAR);
    }
    if(containsChar(sc.c, bracketStreamEndBrackets))
    {
      END_STATEMENT
      VISIT_MISSING(type.c)
      return sc;
    }
    //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    // Check start and end of statements
    if(startStatementNeeded)
    {
      startStatementNeeded = FALSE;
      VISIT_START(SC(';'))
    }
    if(sc.c == ';')
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

      if(sc.c == '/')
      {
        sc = visitSingleLineComment(stream, visitor, sc);
        continue;
      }
      if(sc.c == '*')
      {
        sc = visitMultiLineComment(stream, visitor, sc);
        continue;
      }
      else
        VISIT_CHAR(SC('/'))
    }
    else if(sc.c == '/')
    {
      commentIndicator = TRUE;
      continue;
    }
    //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    // Check for quotes
    if(containsChar(sc.c, bracketStreamQuoteChars))
    {
      sc = visitQuotedToken(stream, visitor, sc);
      continue;
    }
    //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    // Check for start of nested bracket
    if(!containsChar(sc.c, bracketStreamBeginBrackets))
    {
      VISIT_CHAR(sc)
      continue;
    }
    //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    // Start a nested bracket
    const CHAR endBracket = bracketStreamEndBrackets[charStringPos(sc.c, 0, bracketStreamBeginBrackets)];
    sc = visitBracket(stream, visitor, sc, endBracket, nextChar(stream, sc));

    if(sc.c == NO_CHAR)
      continue;
    //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
    // The nested statement terminated incorrectly
    END_STATEMENT

    if(sc.c == endChar)
    {
      VISIT_END(type.c)
      return SC(NO_CHAR);
    }

    VISIT_MISSING(type.c)
    return sc;
    //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
  }
  //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
  // No closing bracket was found
  END_STATEMENT
  VISIT_MISSING(type.c)
  return SC(NO_CHAR);
  //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
}
//-----------------------------------------------------------------------------
static StreamChar visitQuotedToken(BracketStream* const stream,
                                   BracketVisitor* const visitor,
                                   const StreamChar type)
{
  VISIT_START(type)
  VISIT_CHAR(type)

  BOOL escaped = FALSE;
  StreamChar sc;

  for(sc = nextChar(stream, type); sc.c != END_STREAM; sc = nextChar(stream, sc))
  {
    VISIT_CHAR(sc)

    if(sc.c == '\\')
    {
      escaped = !escaped;
      continue;
    }
    if(sc.c == type.c && !escaped)
    {
      VISIT_END(type.c)
      return sc;
    }

    escaped = FALSE;
  }

  VISIT_MISSING(type.c)
  return sc;
}
//-----------------------------------------------------------------------------
static StreamChar visitSingleLineComment(BracketStream* const stream,
                                         BracketVisitor* const visitor,
                                         StreamChar sc)
{
  sc.linePosition--;
  VISIT_START(sc)
  VISIT_CHAR(sc)
  sc.linePosition++;
  VISIT_CHAR(sc)

  for(sc = nextChar(stream, sc); sc.c != END_STREAM; sc = nextChar(stream, sc))
  {
    VISIT_CHAR(sc)

    if(sc.c == '\n')
    {
      VISIT_END('/')
      return sc;
    }
  }

  VISIT_MISSING('/')
  return sc;
}
//-----------------------------------------------------------------------------
static StreamChar visitMultiLineComment(BracketStream* const stream,
                                        BracketVisitor* const visitor,
                                        StreamChar sc)
{
  sc.linePosition--;
  VISIT_START(SC('*'))
  VISIT_CHAR(SC('/'))
  sc.linePosition++;
  VISIT_CHAR(SC('*'))

  BOOL escaped = TRUE;

  for(sc = nextChar(stream, sc); sc.c != END_STREAM; sc = nextChar(stream, sc))
  {
    VISIT_CHAR(sc)

    if(sc.c == '/' && !escaped)
    {
      VISIT_END('*')
      return sc;
    }

    escaped = sc.c != '*';
  }

  VISIT_MISSING('*')
  return sc;
}
//-----------------------------------------------------------------------------