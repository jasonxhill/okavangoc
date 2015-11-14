//=============================================================================
// SyntaxTreeTest.c
//=============================================================================
#include "TestUtil.h"
#include "../src/SyntaxTree.h"
#include "../src/StringBuffer.h"
//-----------------------------------------------------------------------------
static void testSyntaxTree();
//-----------------------------------------------------------------------------
void mainSyntaxTreeTests()
{
  TEST(testSyntaxTree);
}
//-----------------------------------------------------------------------------
static string bracketToString(MemoryPool*, Bracket*);
static string statementToString(MemoryPool*, Statement*);
static string bracketTokenTypeToString(BracketTokenType);
//-----------------------------------------------------------------------------
static void testSyntaxTree()
{
  MemoryPool pool = newMemoryPool();

  string code =
    "statement A;\n"
    "{\n"
    "  // A test comment\n"
    "  statement B;\n"
    "  statement C[1];\n"
    "  /*\n"
    "    multiline comment\n"
    "  */\n"
    "  statement D(r,x) + 20;\n"
    "  \"A test string\";\n"
    "  'A test string';\n"
    "  `A test string`;\n"
    "};\n";

  InMemoryCharStream charStream = newInMemoryCharStream(code);
  SyntaxTree tree = newSyntaxTree(&pool, &charStream.stream);

  StringBuffer sb = newStringBuffer(&pool, "\n");
  string result = sb.append(&sb, bracketToString(&pool, tree.root))
                    .append(&sb, "\n")
                    .toString(&sb, &pool);

  assertEquals(string,
    "\n"
    "START_BRACKET(fileBracket) 1:0\n"
    "START_STATEMENT 1:1\n"
    "statement A\t1:1 - 1:11\n"
    "END_STATEMENT 1:12\n"
    "START_STATEMENT 2:0\n"
    "\n\t2:0 - 2:0\n"
    "START_BRACKET(curlyBrace) 2:1\n"
    "START_STATEMENT 3:0\n"
    "\n  \t3:0 - 3:2\n"
    "START_BRACKET(singleLineComment) 3:3\n"
    "// A test comment\n\n"
    "END_BRACKET(singleLineComment) 4:0\n"
    "  statement B\t4:1 - 4:13\n"
    "END_STATEMENT 4:14\n"
    "START_STATEMENT 5:0\n"
    "\n  statement C\t5:0 - 5:13\n"
    "START_BRACKET(squareBracket) 5:14\n"
    "START_STATEMENT 5:15\n"
    "1\t5:15 - 5:15\n"
    "END_STATEMENT 5:16\n"
    "END_BRACKET(squareBracket) 5:16\n"
    "END_STATEMENT 5:17\n"
    "START_STATEMENT 6:0\n"
    "\n  \t6:0 - 6:2\n"
    "START_BRACKET(multiLineComment) 6:3\n"
    "/*\n"
    "    multiline comment\n"
    "  */\n"
    "END_BRACKET(multiLineComment) 8:4\n"
    "\n  statement D\t9:0 - 9:13\n"
    "START_BRACKET(parentheses) 9:14\n"
    "START_STATEMENT 9:15\n"
    "r,x\t9:15 - 9:17\n"
    "END_STATEMENT 9:18\n"
    "END_BRACKET(parentheses) 9:18\n"
     " + 20\t9:19 - 9:23\n"
    "END_STATEMENT 9:24\n"
    "START_STATEMENT 10:0\n"
    "\n  \t10:0 - 10:2\n"
    "START_BRACKET(doubleQuote) 10:3\n"
    "\"A test string\"\n"
    "END_BRACKET(doubleQuote) 10:17\n"
    "END_STATEMENT 10:18\n"
    "START_STATEMENT 11:0\n"
    "\n  \t11:0 - 11:2\n"
    "START_BRACKET(singleQuote) 11:3\n"
    "'A test string'\n"
    "END_BRACKET(singleQuote) 11:17\n"
    "END_STATEMENT 11:18\n"
    "START_STATEMENT 12:0\n"
    "\n  \t12:0 - 12:2\n"
    "START_BRACKET(backtick) 12:3\n"
    "`A test string`\n"
    "END_BRACKET(backtick) 12:17\n"
    "END_STATEMENT 12:18\n"
    "START_STATEMENT 13:0\n"
    "\n\t13:0 - 13:0\n"
    "END_STATEMENT 13:1\n"
    "END_BRACKET(curlyBrace) 13:1\n"
    "END_STATEMENT 13:2\n"
    "START_STATEMENT 14:0\n"
    "\n\t14:0 - 14:0\n"
    "END_STATEMENT 14:0\n"
    "END_BRACKET(fileBracket) 14:0\n",
    result);

  pool.drain(&pool);
}
//-----------------------------------------------------------------------------
static string bracketToString(MemoryPool* const pool, Bracket* const bracket)
{
  StringBuffer sb = newStringBuffer(pool, "START_BRACKET(");

  sb.append(&sb, bracketTokenTypeToString(bracket->super.super.type))
    .append(&sb, ") ")
    .append(&sb, intToString(pool, bracket->super.super.startPosition.line))
    .append(&sb, ":")
    .append(&sb, intToString(pool, bracket->super.super.startPosition.linePosition))
    .append(&sb, "\n");

  for(Statement** s = bracket->statements; *s != NULL; s++)
    sb.append(&sb, statementToString(pool, *s));

  if(bracket->super.value != NULL)
    sb.append(&sb, bracket->super.value).append(&sb, "\n");

  sb.append(&sb, "END_BRACKET(")
    .append(&sb, bracketTokenTypeToString(bracket->super.super.type))
    .append(&sb, ") ")
    .append(&sb, intToString(pool, bracket->super.super.endPosition.line))
    .append(&sb, ":")
    .append(&sb, intToString(pool, bracket->super.super.endPosition.linePosition));

  return sb.toString(&sb, pool);
}
//-----------------------------------------------------------------------------
static string statementToString(MemoryPool* const pool, Statement* const statement)
{
  StringBuffer sb = newStringBuffer(pool, "START_STATEMENT ");
  sb.append(&sb, intToString(pool, statement->super.startPosition.line))
    .append(&sb, ":")
    .append(&sb, intToString(pool, statement->super.startPosition.linePosition))
    .append(&sb, "\n");

  for(StatementComponent** s = statement->components; *s != NULL; s++)
    if((*s)->super.type != token)
      sb.append(&sb, bracketToString(pool, (Bracket*) *s)).append(&sb, "\n");
    else
      sb.append(&sb, (*s)->value)
        .append(&sb, "\t")
        .append(&sb, intToString(pool, (*s)->super.startPosition.line))
        .append(&sb, ":")
        .append(&sb, intToString(pool, (*s)->super.startPosition.linePosition))
        .append(&sb, " - ")
        .append(&sb, intToString(pool, (*s)->super.endPosition.line))
        .append(&sb, ":")
        .append(&sb, intToString(pool, (*s)->super.endPosition.linePosition))
        .append(&sb, "\n");

  sb.append(&sb, "END_STATEMENT ")
    .append(&sb, intToString(pool, statement->super.endPosition.line))
    .append(&sb, ":")
    .append(&sb, intToString(pool, statement->super.endPosition.linePosition))
    .append(&sb, "\n");

  return sb.toString(&sb, pool);
}
//-----------------------------------------------------------------------------
#define CASE(VAR) case VAR: return #VAR;

static string bracketTokenTypeToString(const BracketTokenType type)
{
  switch(type)
  {
    CASE(curlyBrace)
    CASE(squareBracket)
    CASE(parentheses)
    CASE(fileBracket)
    CASE(singleLineComment)
    CASE(multiLineComment)
    CASE(doubleQuote)
    CASE(singleQuote)
    CASE(backtick)
    CASE(token)
    CASE(unknown)
    default: return "undefined";
  }
}
#undef CASE
//-----------------------------------------------------------------------------